async function sendPostRequest() {
    // Parameters to send
    const params = new URLSearchParams();
    params.append('num1', document.getElementById("inputNumberOne").value);
    params.append('num2', document.getElementById("inputNumberTwo").value);

    try {
        const response = await fetch('/multiplyTwo', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: params.toString(),
        });

        // Check if the response is okay
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        // Parse the response
        const result = await response.text();
        document.getElementById("result").innerText = result;
    } catch (error) {
        console.error('Fetch error:', error);
    }
}