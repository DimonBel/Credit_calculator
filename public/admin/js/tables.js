// Function to fetch JSON data from the API and plot various charts
function fetchDataAndPlotCharts() {
    fetch('/api/data') // Replace with your API endpoint URL
        .then(response => response.json()) // Parse JSON response
        .then(data => {
            // Extract relevant data for different comparisons
            const daeValues = data.map(item => item.dae);
            const daeFicoValues = data.map(item => item.dae_fico);
            const rateValues = data.map(item => item.rate);
            const rateFicoValues = data.map(item => item.rate_fico);
            const monthlyPayValues = data.map(item => item.monthly_pay);
            const monthlyPayFicoValues = data.map(item => item.monthly_pay_fico);
            const commissionValues = data.map(item => item.commission);
            const bankNames = data.map(item => item.bank_name);

            // Plotting the DAE comparison chart (with and without FICO)
            new Chart(document.getElementById("dae-chart"), {
                type: 'bar',
                data: {
                    labels: data.map((item, index) => index + 1),
                    datasets: [
                        {
                            label: 'DAE',
                            backgroundColor: "#3e95cd",
                            data: daeValues,
                            fill: false
                        },
                        {
                            label: 'DAE FICO',
                            backgroundColor: "#8e5ea2",
                            data: daeFicoValues,
                            fill: false
                        }
                    ]
                },
                options: {
                    title: {
                        display: true,
                        text: 'DAE vs DAE FICO over Time'
                    }
                }
            });

            // Plotting the Rate comparison chart (with and without FICO)
            new Chart(document.getElementById("rate-chart"), {
                type: 'bar',
                data: {
                    labels: data.map((item, index) => index + 1),
                    datasets: [
                        {
                            label: 'Rate',
                            backgroundColor: "#3cba9f",
                            data: rateValues,
                            fill: false
                        },
                        {
                            label: 'Rate FICO',
                            backgroundColor: "#e8c3b9",
                            data: rateFicoValues,
                            fill: false
                        }
                    ]
                },
                options: {
                    title: {
                        display: true,
                        text: 'Rate vs Rate FICO over Time'
                    }

                }
            });

            // Plotting the Monthly Pay comparison chart (with and without FICO)
            new Chart(document.getElementById("monthly-pay-chart"), {
                type: 'bar',
                data: {
                    labels: data.map((item, index) => index + 1),
                    datasets: [
                        {
                            label: 'Monthly Pay',
                            backgroundColor: "#c45850",
                            data: monthlyPayValues,
                            fill: false
                        },
                        {
                            label: 'Monthly Pay FICO',
                            backgroundColor: "#7f7f7f",
                            data: monthlyPayFicoValues,
                            fill: false
                        }
                    ]
                },
                options: {
                    title: {
                        display: true,
                        text: 'Monthly Pay vs Monthly Pay FICO over Time'
                    }
                }
            });

            // Plotting the Commission comparison as a polar area chart
            new Chart(document.getElementById("commission-chart"), {
                type: 'polarArea',
                data: {
                    labels: bankNames,
                    datasets: [
                        {
                            label: "Commission",
                            backgroundColor: ["#3e95cd", "#8e5ea2", "#3cba9f", "#e8c3b9", "#c45850"],
                            data: commissionValues
                        }
                    ]
                },
                options: {
                    title: {
                        display: true,
                        text: 'Commission by Region'
                    },
                    scales: {
                        yAxes: [{
                            display: true,
                            ticks: {
                                beginAtZero: true,
                                steps: 0,
                                stepValue: 5,
                                max: 10000
                            }
                        }]
                    },

                }
            });

        })
        .catch(error => {
            console.error('Error fetching or parsing data:', error);
        });
}

// Call the fetchDataAndPlotCharts function when the DOM content is loaded
document.addEventListener('DOMContentLoaded', () => {
    fetchDataAndPlotCharts();
});
