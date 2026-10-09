"""Serve the key reader page and store each results POST to results.json."""
import http.server, json, os, sys

ROOT = os.path.dirname(os.path.abspath(__file__))


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *a, **kw):
        super().__init__(*a, directory=ROOT, **kw)

    def do_POST(self):
        if self.path != '/results':
            return self.send_error(404)
        n = int(self.headers.get('Content-Length', 0))
        if n > 200_000:
            return self.send_error(413)
        body = self.rfile.read(n)
        try:
            data = json.loads(body)
        except ValueError:
            return self.send_error(400)
        tmp = os.path.join(ROOT, 'results.json.tmp')
        with open(tmp, 'w') as f:
            json.dump(data, f, indent=1)
        os.replace(tmp, os.path.join(ROOT, 'results.json'))
        self.send_response(204)
        self.end_headers()

    def log_message(self, fmt, *args):
        if self.command != 'POST':
            super().log_message(fmt, *args)


http.server.ThreadingHTTPServer(('127.0.0.1', int(sys.argv[1])), Handler).serve_forever()
