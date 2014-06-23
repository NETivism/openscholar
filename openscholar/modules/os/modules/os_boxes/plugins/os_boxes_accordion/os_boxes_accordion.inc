<?php

class os_boxes_accordion extends os_boxes_widget_collection {

  public $embed_as = array(
    'iframe',
  );

  function options_defaults() {
    return parent::options_defaults() + array(
      'collapsed' => false,
    );
  }

  function options_form(&$form_state) {
    $form = parent::options_form($form_state);

    $form['collapsed'] = array(
      '#type' => 'checkbox',
      '#default_value' => $this->options['collapsed'],
      '#title' => t('All Panels Closed?'),
      '#description' => t('By default, the first panel of the Accordion will be opened. Checking this will leave it
        closed until the user opens it.')
    );

    return $form;
  }

  function render() {
    $output = parent::render();

    if (($output['content'] = $this->get_cache()) === FALSE) {
      $content = array(
        '#prefix' => '<div class="accordion">',
        '#suffix' => '</div>',
        '#attached' => array(
          'library' => array(
            array('system', 'ui.accordion'),
          ),
          'js' => array(
            drupal_get_path('module', 'os_boxes') . '/plugins/os_boxes_accordion/os_boxes_accordion.render.js',
            array(
              'type' => 'setting',
              'data' => array(
                'os_boxes' => array(
                  'accordion' => array(
                    $this->delta => array(
                      'active' => $this->options['collapsed'] ? FALSE : 0,
                      'delta' => $this->delta,
                    )
                  )
                )
              )
            ),
          ),
        )
      );

      foreach ($this->options['widgets'] as $k => $w) {
        $block_output = $this->render_widget($k);
        unset($block_output['#theme_wrappers']);
        if (!empty($block_output)) {
          $content[$k] = array(
            '#weight' => $w['weight'],
            'title' => array(
              '#prefix' => '<h3>',
              '#suffix' => '</h3>',
              '#markup' => $w['title'],
            ),
            'body' => array(
              '#prefix' => '<div class="accordion-panel">',
              '#suffix' => '</div>',
              'output' => $block_output,
            )
          );
        }
      }

      $output['content'] = drupal_render($content);
      $this->set_cache($output['content'], $content['#attached']);
    }

    return $output;
  }
}
