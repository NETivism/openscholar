<?php

class os_boxes_facetapi_vocabulary extends os_boxes_default {

  /**
   * Create a new box.
   */
  protected function __construct() {
    parent::__construct();
    $this->title = t('Filter by taxonomy');
  }

  /**
   * Implementation of boxes_content::options_defaults().
   */
  public function options_defaults() {
    $options = parent::options_defaults();
    $options['bundle'] = FALSE;
    $options['vocabularies'] = array();

    return $options;
  }

  /**
   * Implementation of boxes_content::options_form().
   */
  public function options_form(&$form_state) {
    $form = parent::options_form($form_state);

    $bundle = $this->options['bundle'];
    $selected_vocab = $this->options['vocabularies'];

    if (!empty($form_state['values']['bundle'])) {
      // We have the bundle from the AJAX.
      $bundle = $form_state['values']['bundle'];
      $selected_vocab = 'all';
    }

    $form['bundle'] = array(
      '#type' => 'select',
      '#description' => t('Which bundle would you like to use.'),
      '#title' => t('Content type'),
      '#options' => os_get_bundles(array(OS_PUBLIC_APP, OS_PRIVATE_APP)),
      '#default_value' => $bundle,
      '#required' => TRUE,
      '#default_value' => $bundle,
      '#ajax' => array(
        'callback' => 'os_boxes_facetapi_vocabulary_ajax_callback',
        'wrapper' => 'vocab-wrapper',
      ),
    );

    $form['vocab_wrapper'] = array(
      '#prefix' => '<div id="vocab-wrapper">',
      '#suffix' => '</div>',
      '#parents' => array('vocab_wrapper'),
    );

    $vocabs = os_search_solr_get_bundle_vocabularies($bundle);
    $options = array('all' => t('All vocabularies')) + $vocabs;

    $form['vocab_wrapper']['vocabularies'] = array(
      '#title' => t('Select vocabularies'),
      '#type' => 'select',
      '#options' => $options,
      '#multiple' => TRUE,
      '#default_value' => $selected_vocab,
    );
    return $form;
  }

  /**
   * Check whether an empty Solr search is needed.
   *
   * @return $browse_page
   *   Solr search page id in case empty search is needed, FALSE otherwise.
   */
  public function is_empty_search_needed() {
    if (!$bundle = $this->options['bundle']) {
      return;
    }

    $page_id = 'browse_' . $bundle;
    if (!$search_page = apachesolr_search_page_load($page_id)) {
      return;
    }
    $env_id = $search_page['env_id'];

    if (apachesolr_has_searched($env_id)) {
      // Search was executed already.
      return;
    }
    return $page_id;
  }

  /**
   * Facet widgets that in a non-solr search page must fake a search page in
   * order to show the facet. To do so we execute a "silent" search on solr
   * without showing the results.
   */
  public function call_empty_search_if_needed() {
    if (!$page_id = $this->is_empty_search_needed()) {
      return;
    }

    // Execute the query.
    try {
      if (!$search_page = apachesolr_search_page_load($page_id)) {
        throw new Exception("Failed to load search page information $search_page");
      }
      $env_id = $search_page['env_id'];

      if (apachesolr_has_searched($env_id)) {
        // Search was executed already.
        return;
      }
      $solr = apachesolr_get_solr($env_id);

      $conditions = apachesolr_search_conditions_default($search_page);

      // Initializes params for empty query.
      $params = array(
        'spellcheck' => 'false',
        'fq' => isset($conditions['fq']) ? $conditions['fq'] : array(),
        'rows' => 1,
      );
      $context['page_id'] = $page_id;
      $context['search_type'] = 'apachesolr_search_show_facets';
      apachesolr_search_run_empty('apachesolr', $params, $search_page['search_path'], $solr, $context);

      // Returning page id means there was an empty search, and we will cache the
      // block.
      return $page_id;
    }
    catch (Exception $e) {
      watchdog('Apache Solr', nl2br(check_plain($e->getMessage())), NULL, WATCHDOG_ERROR);
    }
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render() {
    $block = parent::render();
    $searcher_name = os_search_solr_get_searcher_name();
    // Gets delta map
    if (!$map = facetapi_get_delta_map()) {
      return $block;
    }

    $delta = $block['delta'];

    // Filter by OG vocab.
    $string_og_vocab = facetapi_build_delta($searcher_name, 'block', 'sm_og_vocabulary');
    $delta_og_vocab = array_search($string_og_vocab, $map);

    if ($page_id = $this->is_empty_search_needed()) {
      // Block delta is needed since $delta_og_vocab is not unique between sites
      $cid = 'sm_og_vocabulary:' . $delta . ':' . $delta_og_vocab . ':' . $page_id;
    }
    // Verify the cache is still valid, based on time since created.
    if ($cid && ($cache = cache_get($cid)) && $cache->created > REQUEST_TIME - variable_get('facetapi_vocabulary_cache_timeout', 300)) {
      $block = $cache->data;
    }
    else {
      // Verify the facet block has something to work on (means - it is in a solr
      // search page). If not - execute the empty solr search without showing
      // the results.
      // If empty search happened, we will cache the block.
      $set_cache = $this->call_empty_search_if_needed();

      $block = module_invoke('facetapi', 'block_view', $delta_og_vocab);
      $block['delta'] = $delta;

      // Set block subject/title from box configuration.
      $block['subject'] = check_plain($this->title);
      $block['title'] = check_plain($this->title);

      if (empty($block['content'])) {
        // Block content must be defined, even if it is empty, to avoid errors.
        // os_boxes_widget_submit() renders blocks into a 'dummy' region, but
        // gives errors in case the content is empty, hence the space.
        $block['content'] = ' ';
      }
      else {
        if (!empty($this->options['vocabularies'])) {
          // Get the list of selected vocabularies
          $selected = array();
          foreach ($this->options['vocabularies'] as $vocabulary_name => $value) {
            if ($value == "all") {
              $selected = array();
              break;
            }
            if ($value) {
              $selected[$vocabulary_name] = $value;
            }
          }
          // If no selection - show all. If there is selection - show only them.
          if (count($selected)) {
            foreach (array_keys($block['content']['sm_og_vocabulary']) as $vocabulary_name) {
              if (empty($selected[$vocabulary_name])) {
                unset($block['content']['sm_og_vocabulary'][$vocabulary_name]);
              }
            }
          }
        }

        if (!empty($block['content']['sm_og_vocabulary'])) {
          // Sort by vocab's title.
          ksort($block['content']['sm_og_vocabulary']);
        }

        $block['content'] = render($block['content']);
        if ($set_cache) {
          $cid = 'sm_og_vocabulary:' . $delta . ':' . $delta_og_vocab . ':' . $page_id;
          cache_set($cid, $block);
        }
      }
    }
    return $block;
  }
}

/**
 * AJAX callback to attach the vocab to the form.
 *
 * Since the controlling logic for populating the form is in the form builder
 * function, all we do here is select the element and return it to be updated.
 */
function os_boxes_facetapi_vocabulary_ajax_callback($form, &$form_state) {
  return $form['options']['vocab_wrapper'];
}
