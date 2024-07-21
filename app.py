from flask import Flask, request, redirect, url_for, render_template, send_file
import os
from compress import compress_file
from decompress import decompress_file

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['COMPRESSED_FOLDER'] = 'compressed'
app.config['DECOMPRESSED_FOLDER'] = 'decompressed'
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16 MB limit

os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)
os.makedirs(app.config['COMPRESSED_FOLDER'], exist_ok=True)
os.makedirs(app.config['DECOMPRESSED_FOLDER'], exist_ok=True)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/compress', methods=['POST'])
def compress():
    file = request.files['file']
    if file:
        input_path = os.path.join(app.config['UPLOAD_FOLDER'], file.filename)
        compressed_path = os.path.join(app.config['COMPRESSED_FOLDER'], file.filename + '.huff')
        file.save(input_path)
        compress_file(input_path, compressed_path)
        return send_file(compressed_path, as_attachment=True)
    return redirect(url_for('index'))

@app.route('/decompress', methods=['POST'])
def decompress():
    file = request.files['file']
    if file:
        input_path = os.path.join(app.config['COMPRESSED_FOLDER'], file.filename)
        decompressed_path = os.path.join(app.config['DECOMPRESSED_FOLDER'], file.filename.replace('.huff', '.txt'))
        file.save(input_path)
        decompress_file(input_path, decompressed_path)
        return send_file(decompressed_path, as_attachment=True)
    return redirect(url_for('index'))

if __name__ == "__main__":
    app.run(debug=True)
