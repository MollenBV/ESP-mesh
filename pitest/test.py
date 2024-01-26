from flask import Flask, request

app = Flask(__name__)

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.get_json()
    print(f"Received data: {data}")
    return {"message": "Data received successfully"}

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)