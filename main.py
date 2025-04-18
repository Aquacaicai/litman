from backend.app import create_app


def main():
    host = "127.0.0.1"
    port = "2747"
    app = create_app(f"http://{host}:{port}/")
    app.run(host, port, debug=True)


if __name__ == "__main__":
    main()
