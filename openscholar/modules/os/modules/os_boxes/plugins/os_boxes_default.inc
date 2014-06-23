<?php

/**
 * @file
 * The base class for OS custom widgets. It defines an "abstract box"
 * to be extended by  "real boxes" like "listings", "feeds", "images" etc.
 *
 * It provides the widget visibility optoins. Users will decide where the
 * widget will be shown (i.e. in the current section / feature or everywhere in
 * the site
 *
 * One idea would be to even remove the widget visiblity options from here and
 * let the higher layers to deal with it
 */


abstract class os_boxes_default extends boxes_box {

  /**
   * If this box has been overridden the changed timestamp will be held here
   */
  public $changed;

  /**
   * Holds the cache id.
   */
  protected $cache_id;

  /**
   * Holds the unix time stamp until when the plugin suppose to be cached.
   *
   * The value can be a string which the function strtotime accept i.e 5 minutes
   * from now or a unix time. When set to NULL the caching will be kept until:
   *  - User created/updated or deleted a content.
   *  - Update the plugin settings
   *  - The cache was clear.
   */
  protected $cache_time = NULL;

  /**
   * Given an embed type, return a label
   */
  public function embed_label($type) {
    $types = &drupal_static(__FUNCTION__, array(
      'inline' => t('HTML5'),
      'iframe' => t('Iframe'),
      'oembed' => t('URL')
    ));

    if (isset($types[$type])) {
      return $types[$type];
    }
    return '';
  }

  /**
   * Implementation of boxes_content::options_defaults().
   */
  public function options_defaults() {
    return array(
      'make_embeddable' => false,
    );
  }

  /**
   * Implementation of boxes_content::options_form().
   */
  public function options_form(&$form_state) {
    $form = array();

    $form['make_embeddable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make Embeddable'),
      '#description' => t('Allow this widget to be embedded on other websites.'),
      '#default_value' => (isset($this->options['make_embeddable'])) ? $this->options['make_embeddable'] : FALSE,
      '#access' => !empty($this->embed_as),
      '#weight' => 9001,
    );

    $form['advanced'] = array(
      '#type' => 'fieldset',
      '#access' => user_access('use boxes advanced settings'),
      '#title' => t('Advanced Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 1000,
      '#after_build' => array('_os_boxes_default_advanced_handler'),
    );

    return $form;
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render() {
    // Default cache id set with default (NULL) bundle.
    $this->set_cache_id();

    $title = isset($this->title) ? $this->title : NULL;
    $block['title'] = $title;
    $block['delta'] = $this->delta;
    $block['content'] = '';
    if (!empty($this->options['make_embeddable'])) {
      $block['embed_link'] = true;
    }

    // Get the assets from the cache.
    $block['assets'] = $this->get_assets();
    return $block;
  }

  /**
   * Get the caching of the box.
   *
   * @param $bundle
   *  The bundle of the current widget.
   * @param $page
   *  Passing the page number when we handling a pager.
   *
   * @return String
   *  The cache ID string.
   */
  protected function set_cache_id($bundle = NULL, $page = NULL) {
    if (module_exists('vsite') && !$vsite = vsite_get_vsite()) {
      return;
    }

    $this->cache_id = 'os_boxes_cache:' . $vsite->id;
    if ($bundle && is_string($bundle)) {
      $this->cache_id .= ':' . $bundle;
    }
    $this->cache_id .= ':' . $this->delta;
    if ($page) {
      $this->cache_id .= ':' . $page;
    }

    return $this->cache_id;
  }

  /**
   * Retrieve the cache.
   *
   * @return
   *   The cached data, or FALSE if no cache exists or is expired.
   *   NULL is a valid return value for an empty box.
   */
  protected function get_cache() {
    if (empty($this->cache_id)) {
      // No valid cache id.
      return FALSE;
    }
    if (!$cache = cache_get($this->cache_id, 'cache_os_boxes')) {
      // No cache exists.
      return FALSE;
    }

    if ($cache->expire != CACHE_PERMANENT && $cache->expire < REQUEST_TIME) {
      // Cache object was expired.
      return FALSE;
    }

    if ($assets = $this->get_assets()) {
      $this->add_assets($assets);
    }

    // For Admin users, add indicator that the returned block came from cache.
    if ($cache->data && user_access('administer openscholar')) {
      $cache->data .= "<!--From Cache CID[{$this->cache_id}]-->";
    }

    drupal_add_http_header('x-drupal-cache-os-boxes-plugin', $this->plugin_key, TRUE);
    drupal_add_http_header('x-drupal-cache-os-boxes-cache-id', $this->cache_id, TRUE);

    return $cache->data;
  }

  /**
   * Helper function for retrieve the assets easily.
   */
  protected function get_assets() {
    if ($assets_cache = cache_get('assets:' . $this->cache_id, 'cache_os_boxes')) {
      $assets = unserialize($assets_cache->data);
      return $assets;
    }
  }

  /**
   * Adding the cached assets.
   *
   * @param $assets
   *  Lists of assets.
   */
  protected function add_assets($assets) {
    foreach ($assets as $key => $asset) {
      switch ($key) {
        case 'library' :
          foreach ($asset as $library) {
            drupal_add_library($library[0], $library[1]);
          }
          break;
        case 'js' :
          foreach ($asset as $js) {
            if (!is_array($js)) {
              drupal_add_js($js);
            }
            else {
              drupal_add_js(end($js), reset($js));
            }
          }
          break;
        case 'settings' :
          foreach ($asset as $setting) {
            drupal_add_js($setting, 'setting');
          }
          break;
        case 'css' :
          foreach ($asset as $css) {
            if (!is_array($css)) {
              drupal_add_css($css);
            }
          }
          break;
        default:
          drupal_set_message(t('Invalid cache assets key "%key"', array('%key' => $key)), 'error');
      }
    }
  }

  /**
   * Set the cache for the next time.
   *
   * @param $cache
   *  The information we want to cache.
   * @param $assets
   *  The assets of the box. Optional.
   */
  protected function set_cache($cache, $assets = NULL) {
    // Holds boolean whether rendering is done as dummy. Needs in order to avoid
    // caching of dummy rendering.
    // @todo: remove the dummy rendering fully.
    global $dummy_rendering;
    if ($dummy_rendering) {
      return;
    }

    if (empty($this->cache_id)) {
      // No valid cache id.
      return;
    }

    if (!empty($this->cache_time)) {
      $expire = is_string($this->cache_time) ? strtotime($this->cache_time) : $this->cache_time;
    }
    else {
      $expire = CACHE_PERMANENT;
    }

    cache_set($this->cache_id, $cache, 'cache_os_boxes', $expire);
    if ($assets) {
      $cache_id = 'assets:' . $this->cache_id;
      cache_set($cache_id, serialize($assets), 'cache_os_boxes', $expire);
    }
  }

  /**
   * When submitting a box we need to invalidate here.
   */
  public function options_submit($form, $form_state) {
    if (module_exists('vsite') && !$vsite = vsite_get_vsite()) {
      return;
    }

    // We try to delete the cache of a newly selected bundle, leaving the old cached
    // info unused. If we will re-select the old bundle its cache will be cleared
    // next time.
    if (isset($this->options['content_type']) && is_string($this->options['content_type'])) {
      $bundle = 'node:' . $this->options['content_type'];
    }
    else {
      $bundle = NULL;
    }
    $cid = $this->set_cache_id($bundle);

    cache_clear_all($cid, 'cache_os_boxes', TRUE);
    cache_clear_all('assets:' . $cid, 'cache_os_boxes', TRUE);
  }
}

function _os_boxes_default_advanced_handler($element, $form_state) {
  $children = element_children($element);
  if (empty($children)) {
    $element['#access'] = false;
  }

  return $element;
}
