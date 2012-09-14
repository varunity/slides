
(function ($) {

Drupal.behaviors.slidesFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.slides-form', context).drupalSetSummary(function (context) {
      var val = $('.form-item-slides-bid select').val();

      if (val === '0') {
        return Drupal.t('Not in slides');
      }
      else if (val === 'new') {
        return Drupal.t('New slides');
      }
      else {
        return Drupal.checkPlain($('.form-item-slides-bid select :selected').text());
      }
    });
  }
};

})(jQuery);
