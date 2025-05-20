import os
import gzip
import requests
import time
from tqdm import tqdm
from pathlib import Path
import json
import sseclient


def split_large_xml(input_file, output_dir, target_size_mb=100):
    os.makedirs(output_dir, exist_ok=True)

    top_level_tags = [
        "article", "inproceedings", "proceedings", "book",
        "incollection", "phdthesis", "mastersthesis", "www",
        "person", "data"
    ]

    end_tags = [f"</{tag}>" for tag in top_level_tags]

    xml_header = '<?xml version="1.0" encoding="ISO-8859-1"?>\n<!DOCTYPE dblp SYSTEM "dblp.dtd">\n'
    xml_footer = '</dblp>'

    target_size = target_size_mb * 1024 * 1024

    file_index = 1
    current_file = None
    current_size = 0
    first_file = True

    print(f"splitting {input_file}...")

    total_size = os.path.getsize(input_file)

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as infile:
        line = infile.readline()
        found_dblp_start = False

        while line and not found_dblp_start:
            if '<dblp>' in line:
                found_dblp_start = True
                _, _, remaining = line.partition('<dblp>')
                line = '<dblp>' + remaining
            else:
                line = infile.readline()

        if not found_dblp_start:
            raise ValueError("invalid DBLP XML!")

        output_file = os.path.join(output_dir, f"split_{file_index}.xml")
        current_file = open(output_file, 'w', encoding='utf-8')

        current_file.write(line)
        current_size += len(line.encode('utf-8'))

        with tqdm(total=total_size, unit='B', unit_scale=True, desc="Processing") as pbar:
            processed_bytes = len(line.encode('utf-8'))
            pbar.update(processed_bytes)

            for line in infile:
                line_bytes = line.encode('utf-8')
                line_size = len(line_bytes)
                processed_bytes += line_size
                pbar.update(line_size)

                current_file.write(line)
                current_size += line_size

                if current_size >= target_size:
                    split_here = False
                    split_position = None

                    for end_tag in end_tags:
                        if end_tag in line:
                            tag_positions = []
                            start_pos = 0

                            while True:
                                pos = line.find(end_tag, start_pos)
                                if pos == -1:
                                    break
                                tag_positions.append(pos + len(end_tag))
                                start_pos = pos + len(end_tag)

                            if tag_positions:
                                current_split_pos = tag_positions[-1]

                                if split_position is None or current_split_pos > split_position:
                                    split_position = current_split_pos
                                    split_here = True

                    if split_here and split_position is not None:
                        current_file.seek(0, os.SEEK_END)
                        current_file.seek(
                            current_file.tell() - len(line), os.SEEK_SET)
                        current_file.truncate()

                        current_file.write(line[:split_position])

                        if not first_file:
                            current_file.write(xml_footer)

                        current_file.close()

                        file_index += 1
                        output_file = os.path.join(
                            output_dir, f"split_{file_index}.xml")
                        current_file = open(output_file, 'w', encoding='utf-8')

                        current_file.write(xml_header)
                        current_file.write('<dblp>\n')

                        current_file.write(line[split_position:])

                        current_size = (
                            len(xml_header.encode('utf-8')) +
                            len('<dblp>\n'.encode('utf-8')) +
                            len(line[split_position:].encode('utf-8'))
                        )

                        first_file = False

            if current_file:
                current_file.close()

    print(f"splitted to {file_index} files under {output_dir} folder")
    return file_index


def download_dblp_xml(output_file: Path):
    url = "https://dblp.org/xml/dblp.xml.gz"

    if os.path.exists(output_file):
        print(f"{output_file} exists")
        return

    output_dir = output_file.parent
    if not (os.path.exists(output_dir) and os.path.isdir(output_dir)):
        os.mkdir(output_dir)

    print(f"downloading dblp: {url}")

    response = requests.get(url, stream=True)
    total_size = int(response.headers.get('content-length', 0))

    temp_gz_file = str(output_file) + ".gz"
    with open(temp_gz_file, 'wb') as f, tqdm(
            desc="Downloading",
            total=total_size,
            unit='B',
            unit_scale=True,
            unit_divisor=1024,
    ) as bar:
        for chunk in response.iter_content(chunk_size=1024*1024):
            if chunk:
                f.write(chunk)
                bar.update(len(chunk))

    print("uncompressing")

    with gzip.open(temp_gz_file, 'rb') as f_in:
        with open(output_file, 'wb') as f_out, tqdm(
                desc="Extracting",
                unit='B',
                unit_scale=True,
                unit_divisor=1024,
        ) as bar:
            while True:
                chunk = f_in.read(1024*1024)
                if not chunk:
                    break
                f_out.write(chunk)
                bar.update(len(chunk))

    os.remove(temp_gz_file)
    print(f"uncompressed to {output_file}")


def load_collaboration_cliques(api_root):
    print("getting clique...")

    start_time = time.time()

    try:
        url = f"http://127.0.0.1:2747/api/stats/collaboration/cliques-counts"
        headers = {'Accept': 'text/event-stream'}
        response = requests.get(url, headers=headers, stream=True)

        for line in response.iter_lines():
            line = line.decode('utf-8')
            if line.startswith('data:'):
                event_data = line[5:].strip()
                data = json.loads(event_data)
                if data.get("status") == "done":
                    print("got clique!")
                    break

    except Exception as e:
        print(f"error: {str(e)}")
        return None
    finally:
        total_time = time.time() - start_time
        print(f"running time: {total_time}")
        return total_time


def import_split_xml_files(target_dir, api_root='http://localhost:2747/api', run_pivoter=False, batch_size=None):
    xml_files = [f for f in os.listdir(target_dir) if f.endswith('.xml')]

    xml_files.sort(key=lambda x: int(x.split('_')[1].split('.')[0]))

    if batch_size:
        xml_files = xml_files[:batch_size]

    print(f"Found {len(xml_files)} XML files to import")

    success_count = 0
    fail_count = 0
    total_articles = 0

    import_counts = []
    import_times = []
    pivoter_times = []
    with tqdm(total=len(xml_files), desc="Importing XML files") as pbar:
        for xml_file in xml_files:
            file_path = os.path.join(target_dir, xml_file)
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    xml_content = f.read()

                start_time = time.time()

                response = requests.post(
                    api_root + "/import/xml",
                    data=xml_content.encode('utf-8'),
                    headers={'Content-Type': 'application/xml'}
                )

                import_time = time.time() - start_time

                if response.status_code == 200:
                    result = response.json()
                    if result.get('success'):
                        success_count += 1
                        imported_count = result.get('count', 0)
                        total_articles += imported_count

                        import_counts.append(total_articles)
                        import_times.append(import_time)

                        pivoter_times.append(
                            load_collaboration_cliques(api_root) if run_pivoter else None)

                        tqdm.write(
                            f"Successfully imported {xml_file}: {imported_count} articles, time: {import_time:.2f}s")
                    else:
                        fail_count += 1
                        tqdm.write(
                            f"Failed to import {xml_file}: {result.get('message')}, time: {import_time:.2f}s")
                else:
                    fail_count += 1
                    tqdm.write(
                        f"Error importing {xml_file}: HTTP {response.status_code}, time: {import_time:.2f}s")
                pbar.update(1)
                time.sleep(0.1)
            except Exception as e:
                fail_count += 1
                tqdm.write(f"Exception when importing {xml_file}: {str(e)}")
                pbar.update(1)

    print(f"\nImport completed:")
    print(f"- Total files: {len(xml_files)}")
    print(f"- Successfully imported: {success_count} files")
    print(f"- Failed: {fail_count} files")
    print(f"- Total articles imported: {total_articles}")

    print(import_counts)
    print(import_times)
    print(pivoter_times)

    return {
        'total_files': len(xml_files),
        'success_count': success_count,
        'fail_count': fail_count,
        'total_articles': total_articles
    }


if __name__ == "__main__":
    target_dir = Path("tmp")

    download_dblp_xml(target_dir / "dblp.xml")

    split_dir = target_dir / "splitted"
    '''
    num_files = split_large_xml(
        target_dir / "dblp.xml", split_dir, target_size_mb=100)
    '''
    import_result = import_split_xml_files(split_dir, run_pivoter=True)
    print("imported:", import_result)
