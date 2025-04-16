from backend.app import create_app


def main():
    app = create_app("http://127.0.0.1:2474/")
    app.run("127.0.0.1", "2747", debug=True)


if __name__ == "__main__":
    main()
