function sendPostRequest() {
    const inputNumber = document.getElementById('inputNumber').value;

    fetch('http://127.0.0.1:3000/multiply', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain'
        },
        body: inputNumber
    })
        .then(response => response.text())
        .then(data => {
            document.getElementById('result').innerText = 'Result: ' + data;
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('result').innerText = 'An error occurred.';
        });
}