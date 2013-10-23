/**
 * jQuery behaviors for platform notification feeds.
 */
(function ($) {
  Drupal.behaviors.os_notifications = {
    attach: function (context) {

      // Setup.
      var menuLinkSel = '#os-tour-notifications-menu-link';
      $(menuLinkSel).attr('href', '#').parent('li').append($("<div id='os-tour-notifications-list'/>"));
      var settings = Drupal.settings.os_notifications;
      var container = $('#os-tour-notifications-list');
      if (typeof google == 'undefined') {
        return;
      }
      // @TODO: Add support for multiple feeds.
      var feed = new google.feeds.Feed(settings.url);
      var items = [];
      feed.setNumEntries(settings.max);
      feed.load(function (result) {
        if (!result.error) {
          for (var i = 0; i < result.feed.entries.length; i++) {
            var entry = result.feed.entries[i];
            var item = os_tour_notifications_item(entry);
            items.push(item);
            console.log(item);
          }
        }
      });

      if (typeof hopscotch == 'undefined') {
        return;
      }
      var tour = {
        showPrevButton: true,
        scrollTopMargin: 100,
        id: "os-tour-notifications",
        steps: items
      };

      $('#os-tour-notifications-menu-link').click(function() {
        hopscotch.startTour(tour);
        // Removes animation for each step.
        $('.hopscotch-bubble').removeClass('animated');
        // Allows us to target just this tour in CSS rules.
        $('.hopscotch-bubble').addClass('os-tour-notifications');
      });
      $.get('os/tour/user/' + settings.uid + '/notifications_read');
    }
  };

  /**
   * Converts a Google FeedAPI Integration feed item into a hopscotch step.
   *
   * @param {object} entry
   * @returns {string} output
   */
  function os_tour_notifications_item(entry) {
    var output = "<div class='feed_item'>";
    var date = "";
    if (typeof entry.publishedDate != 'undefined' && entry.publishedDate != '') {
      date = os_tour_notifications_fuzzy_date(entry.publishedDate);
      if (typeof date == 'undefined') {
        date = "";
      } else {
        date = "<span class='date'>" + date + "</span>";
      }
    }
    output += date;
    output += "<span class='description'>" + content + "<span/>";

    //
    var content = entry.content;
    if (typeof entry.contentSnippet != 'undefined') {
      content = entry.contentSnippet;
    }
    output += content;
    output += "<br/><a class='title' target='_blank' href='" + entry.link + "'>Read more &raquo;</a></div>";

    var item = {
      title: entry.title,
      content:output,
      target: document.querySelector("#os-notifications-list"),
      placement: "bottom",
      yOffset: 20
    };
    return item;
  }

  /**
   * Takes an ISO time and returns a string with "time ago" version.
   *
   * @param time
   * @returns {string}
   */
  function os_tour_notifications_fuzzy_date(time) {
    var date = new Date(time),
      diff = (((new Date()).getTime() - date.getTime()) / 1000),
      day_diff = Math.floor(diff / 86400);

    if (isNaN(day_diff) || day_diff < 0 || day_diff >= 31) {
      return;
    }

    return day_diff == 0 && (
      diff < 60 && "just now" ||
        diff < 120 && "1 minute ago" ||
        diff < 3600 && Math.floor(diff / 60) + " minutes ago" ||
        diff < 7200 && "1 hour ago" ||
        diff < 86400 && Math.floor(diff / 3600) + " hours ago") ||
      day_diff == 1 && "Yesterday" ||
      day_diff < 7 && day_diff + " days ago" ||
      day_diff < 31 && Math.ceil(day_diff / 7) + " weeks ago";
  }

  /**
   * Updates the notifications count of remaining notifications.
   */
  function os_tour_notifications_count(num_remaining) {
    var count = '#os-tour-notifications-count';
    if (arguments.length === 0) {
      var value = $(count).text();
      return parseInt(value);
    }
    if (parseInt(num_remaining) === 0) {
      $(count).hide();
      return;
    }
    if (parseInt(num_remaining) > 0) {
      $(count).show();
      $(count).text(num_remaining);
    }
  }

  /**
   * Sets the current user's "notifications_read" to the current time.
   *
   * Invoked when a user clicks "Done" on the final tour step.
   */
  function os_tour_notifications_read_update() {
    var settings = Drupal.settings.os_notifications;
    $.get('os/tour/user/' + settings.uid + '/notifications_read');
  }

})(jQuery);
