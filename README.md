<h1> Byway</h1>

A minimal HTTP server written in C, built as a learning project to understand sockets,
parsing, memory management, and the HTTP protocol from the bytes up.

<h2> What it does</h2>

- Serves static files from the `html/` directory over `GET`
- Maps common file extensions to correct `Content-Type` headers
  (`html`, `css`, `js`, `jpg`, `png`, `gif`, `ico`, `json`, `pdf`)
- `POST /add` with an `application/x-www-form-urlencoded` body
  (`x=` and `y=` fields) returns their sum
- Serves `404` and `500` pages when routing or the filesystem goes wrong
- Reads files with a growable buffer (no fixed size limits)

<h2> Build & run</h2>

```sh
make            # compiles and starts the server on port 8800
make testing    # same, but built with AddressSanitizer + UBSan
```

Serve content by placing files under <code>html/</code> 
