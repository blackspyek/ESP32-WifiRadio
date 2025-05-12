from flask import Flask, request, Response
from PIL import Image
import requests
import struct
import io

app = Flask(__name__)

def get_itunes_song_image(song_name, artist_name=None, high_res=False):
    query = f"{song_name} {artist_name}" if artist_name else song_name
    url = "https://itunes.apple.com/search"
    params = {
        "term": query,
        "media": "music",
        "limit": 1,
    }

    try:
        response = requests.get(url, params=params, timeout=5)
        response.raise_for_status()
        results = response.json().get("results")
        if results:
            artwork_url = results[0].get("artworkUrl100")
            if artwork_url and high_res:
                return artwork_url.replace("100x100bb", "600x600bb")
            return artwork_url
    except:
        return None

def convert_image_to_rgb888(url):
    # Download and convert image to RGB888 format
    response = requests.get(url, timeout=5)
    response.raise_for_status()
    img = Image.open(io.BytesIO(response.content))
    img = img.convert("RGB").resize((100, 100))

    raw = io.BytesIO()
    for r, g, b in img.getdata():
        raw.write(bytes([r, g, b]))
    return raw.getvalue()

@app.route("/process")
def process_image():
    url = request.args.get("url")
    if not url:
        return "Missing URL", 400
    try:
        rgb_data = convert_image_to_rgb888(url)
        return Response(rgb_data, mimetype='application/octet-stream')
    except Exception as e:
        return str(e), 500

@app.route("/process_from_itunes")
def process_from_itunes():
    title = request.args.get("title")
    author = request.args.get("author")

    if not title:
        return "Missing title", 400

    image_url = get_itunes_song_image(title, author)
    if not image_url:
        image_url = get_itunes_song_image(title)

    if not image_url:
        return "No image found on iTunes", 404

    try:
        rgb_data = convert_image_to_rgb888(image_url)
        return Response(rgb_data, mimetype='application/octet-stream')
    except Exception as e:
        return str(e), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=3000)
