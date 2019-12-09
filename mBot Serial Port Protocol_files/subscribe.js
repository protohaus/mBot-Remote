$(function () {
    $('#subscribem').on('click', function() {
        $('#subscribem').attr("disabled", "disabled");
        var arr = $('#footerform').serializeArray();
        $.ajax({
            'async'     : false,
            'url'       : 'http://www.makeblock.com/mailsubscibe/index.php',
            'dataType'  : 'jsonp',
            'jsonp'     : 'callback',
            'data'      : arr,
            'timeout'   : 30000,
            'success'   : function (response) {
                alert(response.msg);
             },
             'error': function(request) {
                alert("The net connection is slow!");
            },
        });
        $('#subscribem').removeAttr("disabled");
        return;
    });
});
