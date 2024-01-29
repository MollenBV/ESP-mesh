import requests

# Replace 'raspberry_pi_address' with the actual IP address of your Raspberry Pi
url = 'http://192.168.178.12:5000/data'

# Example payload you want to send as JSON
payload = {
    'key': 'value',
    'example_number': 123,
    'example_list': [1, 2, 3]
}

# Sending a POST request with the JSON payload
response = requests.post(url, json=payload)

# Print the response from the server
print(f'Status Code: {response.status_code}')
print(f'Response JSON: {response.json()}')
