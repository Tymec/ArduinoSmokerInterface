const FAILED_THRESHOLD = 15;
const CHART_DATA_LENGTH = 7;
var failedUpdateDataCount = 0;
var failedUpdateSettingsCount = 0;
var currentState = 'idle';
var WEBSITE_READY = false;

var ctx = document.getElementById('smoker-chart').getContext('2d');
var chart = new Chart(ctx, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature',
            borderColor: 'rgb(255, 169, 231)',
            data: []
        }, {
            label: 'Power Relay',
            borderColor: 'rgb(161, 103, 214)',
            data: [],
            lineTension: 0,
            steppedLine: true        
        },
        {
            label: 'Max Temperature',
            borderColor: 'rgb(255, 0, 0)',
            backgroundColor: 'rgb(0, 0, 0, 0)',
            steppedLine: true,
            pointRadius: 0,
            borderDash: [5],
            borderWidth: 1,
            data: [],
            showLine: false
        },
        {
           label: 'Min Temperature',
           borderColor: 'rgb(0, 255, 0)',
           backgroundColor: 'rgb(0, 0, 0, 0)',
           steppedLine: true,
           pointRadius: 0,
           borderDash: [5],
           borderWidth: 1,
           data: [],
           showLine: false
        }]
    },

    // Configuration options go here
    options: {
        legend: {
            labels: {
                filter: function(legendItem, chartData) {
                    if (legendItem.datasetIndex === 2 || legendItem.datasetIndex === 3) {
                        return false;
                    }
                    return true;
                }
            }
        }
    }
});

$(document).ready(function(event) {
    var posting = $.ajax({
        type: 'GET',
        url: `http://192.168.1.52/api/controller/values?token=6jUKI1F7NvhWoNF`,
        headers: {},
        timeOut: 10000
    });

    posting.done(function(data, textStatus, xhr) {
        if (xhr.status === 200) {
            $('#current-state').find('h1').text(data['controller']['state'].toUpperCase());
            $('#current-temperature').find('h2').text(`${data['controller']['temperature'].toFixed(2)}°C`);

            $('#power-relay-value').text(data['relays']['power'] == true ? "On" : "Off");
            $('#smoking-relay-value').text(data['relays']['smoking'] == true ? "On" : "Off");

            $('#time-left-power').contents().first()[0].textContent = `${data['times']['power']}min left `;
            $('#time-left-smoking').text(`${data['times']['smoking']}min left`);

            $('#smoker_ui').removeAttr('hidden');
            $('.loader').attr('hidden', true);

            setupToast();
            updateSettings();
            WEBSITE_READY = true;
            setInterval(updateData, 15000);
        } else {
            $('.loader').remove();
            showError("Reload the page.");
            toastr.error('Failed to fetch values.');
            console.log("onReady: Failed to fetch values.");
        }
    });

    posting.fail(function() {
        $('.loader').remove();
        showError("Reload the page.");
        toastr.error('Failed to make a GET request.');
        console.log("onReady: Failed to make a GET request.");
    });
});

$('#navbar-home-tab').bind('click', function(e) {
    e.preventDefault();
    if (!WEBSITE_READY) { return; }
    
    $('#smoker_settings').attr('hidden', true);
    $('#smoker_ui').removeAttr('hidden');
});

$('#navbar-settings-tab').bind('click', function(e) {
    e.preventDefault();
    if (!WEBSITE_READY) { return; }
    
    $('#smoker_ui').attr('hidden', true);
    $('#smoker_settings').removeAttr('hidden');
    updateSettings()
});

function updateData() {
    if (failedUpdateDataCount > FAILED_THRESHOLD) {
        showError("Reload the page.", true);
        toastr.error('Fail count too high.');
        console.log("Failed Update Data requests: " + failedUpdateDataCount);
        return;
    }

    var posting = $.ajax({
        type: 'GET',
        url: `http://192.168.1.52/api/controller/values?token=6jUKI1F7NvhWoNF`,
        headers: {},
        timeOut: 10000
    });

    posting.done(function(data, textStatus, xhr) {
        if (xhr.status === 200) {
            $('#current-state').find('h1').text(data['controller']['state'].toUpperCase());
            $('#current-temperature').find('h2').text(`${data['controller']['temperature'].toFixed(2)}°C`);
            $('#power-relay-value').text(data['relays']['power'] == true ? "On" : "Off");
            $('#smoking-relay-value').text(data['relays']['smoking'] == true ? "On" : "Off");
            $('#time-left-power').contents().first()[0].textContent = `${data['times']['power']}min left `;
            $('#time-left-smoking').text(`${data['times']['smoking']}min left`);
            updateChart(data);
            //toastr.success('Updated data.');
        } else {
            console.log("updateData(): Failed to fetch values.");
            toastr.error('Failed to fetch values.');
            failedUpdateDataCount++;
        }
    });

    posting.fail(function() {
        failedUpdateDataCount++;
        console.log("updateData(): Failed to make a GET request.");
        toastr.error('Failed to make a GET request.');
    });
}

function updateSettings() {
    if (failedUpdateSettingsCount > FAILED_THRESHOLD) {
        showError("Reload the page.", true);
        toastr.error('Fail count too high.');
        console.log("Failed Update Data requests: " + failedUpdateDataCount);
        return;
    }
    
    var posting = $.ajax({
        type: 'GET',
        url: `http://192.168.1.52/api/settings/values?token=6jUKI1F7NvhWoNF`,
        headers: {},
        timeOut: 10000
    });

    posting.done(function(data, textStatus, xhr) {
        if (xhr.status === 200) {   
            $('#temps-drying-min-input').val(data['temperatures']['drying']['min']);
            $('#temps-drying-max-input').val(data['temperatures']['drying']['max']);
            
            $('#temps-smoking-min-input').val(data['temperatures']['smoking']['min']);
            $('#temps-smoking-max-input').val(data['temperatures']['smoking']['max']);
            
            $('#temps-thermal-min-input').val(data['temperatures']['thermal']['min']);
            $('#temps-thermal-max-input').val(data['temperatures']['thermal']['max']);
            
            $('#times-drying-input').val(data['times']['drying']);
            $('#times-smoking-input').val(data['times']['smoking']);
            $('#times-turbo-input').val(data['times']['turbo']);
            $('#times-thermal-input').val(data['times']['thermal']);
            
            toastr.success('Settings updated.');
        } else {
            console.log("updateSettings(): Failed to fetch settings.");
            toastr.error('Failed to fetch settings.');
            failedUpdateSettingsCount++;
        }
    });

    posting.fail(function() {
        failedUpdateSettingsCount++;
        console.log("updateSettings(): Failed to make a GET request.");
        toastr.error('Failed to make a GET request.');
    });
}

function updateChart(data) {
    if (currentState != data['controller']['state']) {
        currentState = data['controller']['state'];

        if (currentState === 'IDLE') {
            chart.data.datasets[2].showLine = false;
            chart.data.datasets[3].showLine = false;
        } else {
            chart.data.datasets[2].showLine = true;
            chart.data.datasets[3].showLine = true;
            chart.data.datasets[2].data = new Array(CHART_DATA_LENGTH + 1).fill(data['temperatures']['max_temperature']);
            chart.data.datasets[3].data = new Array(CHART_DATA_LENGTH + 1).fill(data['temperatures']['min_temperature']);
        }
    }
    
    var now = new Date(Date.now());
    var timeNow = ("0" + now.getHours()).slice(-2) + ":" + ("0" + now.getMinutes()).slice(-2) + ":" + ("0" + now.getSeconds()).slice(-2);

    if (chart.data.datasets[0].data.length > CHART_DATA_LENGTH && chart.data.datasets[1].data.length > CHART_DATA_LENGTH) {
        chart.data.datasets[0].data.shift();
        chart.data.datasets[1].data.shift();
        chart.data.labels.shift();
    }

    chart.data.datasets[0].data.push(data['controller']['temperature'].toFixed(2));
    chart.data.datasets[1].data.push(data['relays']['power'] == true ? 10 : 0);
    chart.data.labels.push(timeNow);
    chart.update();
}

function setupToast() {
    toastr.options = {
        "closeButton": false,
        "debug": false,
        "newestOnTop": false,
        "progressBar": true,
        "positionClass": "toast-bottom-right",
        "preventDuplicates": true,
        "onclick": null,
        "showDuration": "300",
        "hideDuration": "500",
        "timeOut": "2000",
        "extendedTimeOut": "500",
        "showEasing": "swing",
        "hideEasing": "linear",
        "showMethod": "fadeIn",
        "hideMethod": "fadeOut"
    }
}

function showError(err, disableAll=false) {
    if (disableAll) {
        $('#smoker_ui').attr('hidden', true);
        $('#smoker_settings').attr('hidden', true);
    }
    $('#error_box').removeAttr('hidden');
    $('#error_box').find('p').text(err);
}

$('#settings-change-form').submit(function(event) {
    event.preventDefault();
    
    $('#update_settings_button').find('span').removeAttr('hidden');
    
    var data = {
        time_drying: $('#times-drying-input').val(),
        time_smoking: $('#times-smoking-input').val(),
        time_thermal: $('#times-thermal-input').val(),
        time_turbo: $('#times-turbo-input').val(),
        
        temp_min_drying: $('#temps-drying-min-input').val(),
        temp_max_drying: $('#temps-drying-max-input').val(),
        temp_min_smoking: $('#temps-smoking-min-input').val(),
        temp_max_smoking: $('#temps-smoking-max-input').val(),
        temp_min_thermal: $('#temps-thermal-min-input').val(),
        temp_max_thermal: $('#temps-thermal-max-input').val()
    };
    
    var posting = $.ajax({
        type: 'POST',
        url: `http://192.168.1.52/api/settings/values?token=6jUKI1F7NvhWoNF`,
        data: data,
        headers: {}
    });
    
    posting.done(function(data, textStatus, xhr) {
        $('#update_settings_button').find('span').attr('hidden', true);

        if (xhr.status === 200) {
            $('#change_state_btn').find('span').attr('hidden', true);
            
            $('#temps-drying-min-input').val(data['temperatures']['drying']['min']);
            $('#temps-drying-max-input').val(data['temperatures']['drying']['max']);
            
            $('#temps-smoking-min-input').val(data['temperatures']['smoking']['min']);
            $('#temps-smoking-max-input').val(data['temperatures']['smoking']['max']);
            
            $('#temps-thermal-min-input').val(data['temperatures']['thermal']['min']);
            $('#temps-thermal-max-input').val(data['temperatures']['thermal']['max']);
            
            $('#times-drying-input').val(data['times']['drying']);
            $('#times-smoking-input').val(data['times']['smoking']);
            $('#times-turbo-input').val(data['times']['turbo']);
            $('#times-thermal-input').val(data['times']['thermal']);
            
            toastr.success('Settings updated.');
        } else {
            console.log("onSettings: Failed to set values.");
        }
    });
    posting.fail(function() {
        $('#update_settings_button').find('span').attr('hidden', true);
        toastr.error('Failed to make a POST request.');
        console.log("onSettings: Failed to make a POST request.");
    });
});

$('#state-change-form').submit(function(event) {
    event.preventDefault();

    var state = $('#state-v').val();

    $('#change_state_btn').find('span').removeAttr('hidden');

    var posting = $.ajax({
        type: 'POST',
        url: `http://192.168.1.52/api/controller/state?v=${state}&token=6jUKI1F7NvhWoNF`,
        headers: {}
    });
    
    posting.done(function(data, textStatus, xhr) {
        $('#change_state_btn').find('span').attr('hidden', true);

        if (xhr.status === 200) {
            var jsonResponse = data['data'];
            $('#current-state').find('h1').text(jsonResponse['value'].toUpperCase());
            $('#time-left-power').contents().first()[0].textContent = `${jsonResponse['time_left']}${jsonResponse['unit']} left `;
            $('#time-left-smoking').text(`${(jsonResponse['smoking_time_left'] == undefined) ? 0 : jsonResponse['smoking_time_left']}${jsonResponse['unit']} left`);
            //updateData();
        } else {
            console.log("onForm: Failed to set values.");
        }
    });
    posting.fail(function() {
        $('#change_state_btn').find('span').attr('hidden', true);
        toastr.error('Failed to make a POST request.');
        console.log("onForm: Failed to make a POST request.");
    });
});
