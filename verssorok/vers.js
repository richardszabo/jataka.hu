(function($) {
    $.fn.embeddable = function(options) {
    // assuming that there is an audio element inside the selected element (this)
        var res = this.each(function(){
            $(this).children("audio").wrap("<div class='audio'></div>");
            var r= $('<input type="button" value="Embed" class="embed-button btn-default"/>');
            $(this).append(r);                
        });
        var btns = document.querySelectorAll('.embed-button');
        new Clipboard(btns,{
            text: function(trigger) {
                return trigger.previousSibling.previousSibling.innerHTML;
            }
        });
        return res;
    };    
})(jQuery);