# litman

Data Structure Course Project: Lit(erature) Man(agement) (System).

## Features

- Importing literatures from [DBLP](https://dblp.org/) XML format datasets.
- Search literatures by author, title, keyword(extracted from title) supported by B+ tree.
- Count and sort the number of publications by all authors.
- Calculate the top 10 keywords by frequencies for each year.
- Obtain the collaboration network of authors.
- Calculate the exact number of cliques in the author collaboration graph, supported by the [Pivoter algorithm](https://arxiv.org/abs/2001.06784).

## Install

We use [uv](https://github.com/astral-sh/uv) to manage project dependencies. If you don't have uv installed, please follow the instructions on the [uv installation page](https://docs.astral.sh/uv/getting-started/installation/).

The B+ tree and Pivoter algorithm are implemented in C++ and use CMake as the build system. They are accessed from Python using [pybind11](https://github.com/pybind/pybind11). Additionally, the Pivoter algorithm uses the [GMP library](https://gmplib.org/) for some calculations, so please make sure you have installed cmake and the GMP library. For these reasons, we recommend building in a Linux environment.

The web frontend is built using GitHub Actions, and the built static pages have already been uploaded to the repository, so you don't need to worry about this.

Install dependencies:

```sh
uv sync
```

Enter uv venv:

```sh
uv venv
```

## Run

After entering uv venv, run

```sh
python main.py
```

Then open the URL bound to Flask, which is [http://127.0.0.1:2747](http://127.0.0.1:2747) by default.

## Load DBLP dataset

After launching Litman, simply run

```python
python load_full_dblp.py
```

to download, unarchive, split, and finally load DBLP dataset to Litman.

Or, you can use the "Import Literature" feature under "Manage Literature" tab in our web frontend to load DBLP XML format datasets manually.
