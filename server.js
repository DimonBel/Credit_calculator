const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');
const fs = require('fs');
const { exec } = require('child_process');

const app = express();
const port = 3000;

// Middleware to parse JSON and URL-encoded form data
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Serve static files from the 'public' directory (if needed)
app.use(express.static(path.join(__dirname, 'public')));

// Route to handle form submission and save data to a file
app.post('/save-data', (req, res) => {
  const { name, email, username, password, Dodic, Pes, ssmid, I_love, Types } = req.body;
  const data = `${name},${email},${username},${password},${Dodic},${Pes},${ssmid},${I_love},${Types}\n`;
  const filePath = path.join(__dirname, 'input.txt');

  // Write form data to the file (overwrite existing content)
  fs.writeFile(filePath, data, (err) => {
    if (err) {
      console.error('Error writing data to file:', err);
      res.status(500).send('Internal Server Error');
    } else {
      console.log('Data written to file:', data);
      executeCProgram(filePath, res); // Call executeCProgram after saving data
    }
  });
});

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'index.html'));
});

// Function to execute the C program with input file and handle output
function executeCProgram(filePath, res) {
  const executablePath = path.join(__dirname, 'main.exe'); // Path to your compiled C program
  const outputFilePath = path.join(__dirname, 'output.json');
  const outputFilePath1 = path.join(__dirname, 'special.json');
  // Execute the C program without input/output redirection
  const command = `"${executablePath}" "${filePath}" "${outputFilePath}"`;

  exec(command, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing C program: ${error.message}`);
      res.status(500).send('Internal Server Error');
      return;
    }
    if (stderr) {
      console.error(`C program returned an error: ${stderr}`);
      res.status(500).send('C Program Error');
      return;
    }

    console.log('C program executed successfully');

    // Read the output JSON file and send its content as response
    fs.readFile(outputFilePath, 'utf8', (err, data) => {
      if (err) {
        console.error('Error reading output file:', err);
        res.status(500).send('Internal Server Error');
      } else {
        let outputData;
        try {
          res.json(outputData); // Send the output data back as a response
        } catch (parseError) {
          console.error('Error parsing output JSON:', parseError);
          res.status(500).send('Internal Server Error');
        }
      }
    });
  });
}

// Route to serve the first 5 credits from output.json
app.get('/api/data', (req, res) => {
  try {
    const outputFilePath = path.join(__dirname, 'output.json');
    const data = fs.readFileSync(outputFilePath, 'utf8');
    const jsonData = JSON.parse(data);
    const firstFiveCredits = jsonData.slice(0, 10);
    res.json(firstFiveCredits);
  } catch (err) {
    console.error('Error reading file:', err);
    res.status(500).json({ error: 'Failed to read data' });
  }
});

app.get('/api/data1', (req, res) => {
  try {
    const outputFilePath1 = path.join(__dirname, 'special.json');
    const data = fs.readFileSync(outputFilePath1, 'utf8');
    const jsonData = JSON.parse(data);
    const firstFiveCredits = jsonData.slice(0, 12);
    res.json(firstFiveCredits);
  } catch (err) {
    console.error('Error reading file:', err);
    res.status(500).json({ error: 'Failed to read data' });
  }
});


// Start the server
app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
