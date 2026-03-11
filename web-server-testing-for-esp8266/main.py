from flask import Flask, render_template, request, jsonify
app = Flask(__name__)

latest_data = {}

@app.route('/data', methods=['POST'])
def receive_data():
    content = request.get_json()
    latest_data.update(content)
    return jsonify({"success": True})

@app.route('/')
def index():
    return render_template('dashboard.html', data=latest_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=10000)
