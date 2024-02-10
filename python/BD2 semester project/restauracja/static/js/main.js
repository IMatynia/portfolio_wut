$(document).ready(function () {
    $('[data-toggle="tooltip"]').tooltip()
})

function addNStools(tableDiv, N) {
    console.log("Adding " + N + " stools to " + tableDiv)
    // Find the container div
    var $container = $(tableDiv);

    const DIST_MULT = 0.8;
    var angle_step = Math.PI * 2 / N;
    // Loop to add N new divs
    for (var i = 1; i <= N; i++) {
        var x_offset = Math.sin(i * angle_step) * 100 * DIST_MULT + 50;
        var y_offset = Math.cos(i * angle_step) * 100 * DIST_MULT + 50;

        var $newDiv = $('<div></div>');
        $newDiv.addClass('stool');
        $newDiv.css({
            'left': x_offset + '%',
            'top': y_offset + '%',
            'transform': 'translate(-50%, -50%) rotate(-' + i * angle_step + 'rad)'
        })
        // Append the new div to the container div
        $container.append($newDiv);
    }
}

function sendDataToEndpointAndReload(endpoint, payload) {
    $.ajax({
        url: endpoint,
        type: "GET",
        data: payload,
        success: function (response) {
            if (response["error"] === null) {
                location.reload(); // Reload the current page
            } else {
                alert(response["error"]);
            }
        },
        error: function (xhr, status, error) {
            alert(error);
        }
    });
}

function exctractPayloadFromProperties(object, data_tags) {
    let payload = {};
    for (const i in data_tags) {
        let data_tag = data_tags[i];
        console.log(data_tag);
        console.log(object.data(data_tag))
        payload[data_tag] = object.data(data_tag);
    }
    return payload;
}

function sendDataToEndpointAndRedirect(endpoint, payload, redirect) {
    $.ajax({
        url: endpoint,
        type: "GET",
        data: payload,
        success: function (response) {
            if (response["error"] === null) {
                window.location.href = redirect;
            } else {
                alert(response["error"]);
            }
        },
        error: function (xhr, status, error) {
            alert(error);
        }
    });
}