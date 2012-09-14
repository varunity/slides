<?php

/**
 * @file
 * Admin page callbacks for the slides module.
 */

/**
 * Returns an administrative overview of all slidess.
 */
function slides_admin_overview() {
  $rows = array();

  $headers = array(t('Book'), t('Operations'));

  // Add any recognized slidess to the table list.
  foreach (slides_get_slidess() as $slides) {
    $rows[] = array(l($slides['title'], $slides['href'], $slides['options']), l(t('edit order and titles'), 'admin/content/slides/' . $slides['nid']));
  }

  return theme('table', array('header' => $headers, 'rows' => $rows, 'empty' => t('No slidess available.')));
}

/**
 * Builds and returns the slides settings form.
 *
 * @see slides_admin_settings_validate()
 *
 * @ingroup forms
 */
function slides_admin_settings() {
  $types = node_type_get_names();
  $form['slides_allowed_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types allowed in slides outlines'),
    '#default_value' => variable_get('slides_allowed_types', array('slides')),
    '#options' => $types,
    '#description' => t('Users with the %outline-perm permission can add all content types.', array('%outline-perm' => t('Administer slides outlines'))),
    '#required' => TRUE,
  );
  $form['slides_child_type'] = array(
    '#type' => 'radios',
    '#title' => t('Content type for child pages'),
    '#default_value' => variable_get('slides_child_type', 'slides'),
    '#options' => $types,
    '#required' => TRUE,
  );
  $form['array_filter'] = array('#type' => 'value', '#value' => TRUE);
  $form['#validate'][] = 'slides_admin_settings_validate';

  return system_settings_form($form);
}

/**
 * Validate the slides settings form.
 *
 * @see slides_admin_settings()
 */
function slides_admin_settings_validate($form, &$form_state) {
  $child_type = $form_state['values']['slides_child_type'];
  if (empty($form_state['values']['slides_allowed_types'][$child_type])) {
    form_set_error('slides_child_type', t('The content type for the %add-child link must be one of those selected as an allowed slides outline type.', array('%add-child' => t('Add child page'))));
  }
}

/**
 * Build the form to administrate the hierarchy of a single slides.
 *
 * @see slides_admin_edit_submit()
 *
 * @ingroup forms.
 */
function slides_admin_edit($form, $form_state, $node) {
  drupal_set_title($node->title);
  $form['#node'] = $node;
  _slides_admin_table($node, $form);
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save slides pages'),
  );

  return $form;
}

/**
 * Check that the slides has not been changed while using the form.
 *
 * @see slides_admin_edit()
 */
function slides_admin_edit_validate($form, &$form_state) {
  if ($form_state['values']['tree_hash'] != $form_state['values']['tree_current_hash']) {
    form_set_error('', t('This slides has been modified by another user, the changes could not be saved.'));
  }
}

/**
 * Handle submission of the slides administrative page form.
 *
 * This function takes care to save parent menu items before their children.
 * Saving menu items in the incorrect order can break the menu tree.
 *
 * @see slides_admin_edit()
 * @see menu_overview_form_submit()
 */
function slides_admin_edit_submit($form, &$form_state) {
  // Save elements in the same order as defined in post rather than the form.
  // This ensures parents are updated before their children, preventing orphans.
  $order = array_flip(array_keys($form_state['input']['table']));
  $form['table'] = array_merge($order, $form['table']);

  foreach (element_children($form['table']) as $key) {
    if ($form['table'][$key]['#item']) {
      $row = $form['table'][$key];
      $values = $form_state['values']['table'][$key];

      // Update menu item if moved.
      if ($row['plid']['#default_value'] != $values['plid'] || $row['weight']['#default_value'] != $values['weight']) {
        $row['#item']['plid'] = $values['plid'];
        $row['#item']['weight'] = $values['weight'];
        menu_link_save($row['#item']);
      }

      // Update the title if changed.
      if ($row['title']['#default_value'] != $values['title']) {
        $node = node_load($values['nid']);
        $langcode = LANGUAGE_NONE;
        $node->title = $values['title'];
        $node->slides['link_title'] = $values['title'];
        $node->revision = 1;
        $node->log = t('Title changed from %original to %current.', array('%original' => $node->title, '%current' => $values['title']));

        node_save($node);
        watchdog('content', 'slides: updated %title.', array('%title' => $node->title), WATCHDOG_NOTICE, l(t('view'), 'node/' . $node->nid));
      }
    }
  }

  drupal_set_message(t('Updated slides %title.', array('%title' => $form['#node']->title)));
}

/**
 * Build the table portion of the form for the slides administration page.
 *
 * @see slides_admin_edit()
 */
function _slides_admin_table($node, &$form) {
  $form['table'] = array(
    '#theme' => 'slides_admin_table',
    '#tree' => TRUE,
  );

  $tree = slides_menu_subtree_data($node->slides);
  $tree = array_shift($tree); // Do not include the slides item itself.
  if ($tree['below']) {
    $hash = drupal_hash_base64(serialize($tree['below']));
    // Store the hash value as a hidden form element so that we can detect
    // if another user changed the slides hierarchy.
    $form['tree_hash'] = array(
      '#type' => 'hidden',
      '#default_value' => $hash,
    );
    $form['tree_current_hash'] = array(
      '#type' => 'value',
      '#value' => $hash,
    );
    _slides_admin_table_tree($tree['below'], $form['table']);
  }

}

/**
 * Recursive helper to build the main table in the slides administration page form.
 *
 * @see slides_admin_edit()
 */
function _slides_admin_table_tree($tree, &$form) {
  // The delta must be big enough to give each node a distinct value.
  $count = count($tree);
  $delta = ($count < 30) ? 15 : intval($count / 2) + 1;

  foreach ($tree as $data) {
    $form['slides-admin-' . $data['link']['nid']] = array(
      '#item' => $data['link'],
      'nid' => array('#type' => 'value', '#value' => $data['link']['nid']),
      'depth' => array('#type' => 'value', '#value' => $data['link']['depth']),
      'href' => array('#type' => 'value', '#value' => $data['link']['href']),
      'title' => array(
        '#type' => 'textfield',
        '#default_value' => $data['link']['link_title'],
        '#maxlength' => 255,
        '#size' => 40,
      ),
      'weight' => array(
        '#type' => 'weight',
        '#default_value' => $data['link']['weight'],
        '#delta' => max($delta, abs($data['link']['weight'])),
        '#title' => t('Weight for @title', array('@title' => $data['link']['title'])),
        '#title_display' => 'invisible',
      ),
      'plid' => array(
        '#type' => 'hidden',
        '#default_value' => $data['link']['plid'],
      ),
      'mlid' => array(
        '#type' => 'hidden',
        '#default_value' => $data['link']['mlid'],
      ),
    );
    if ($data['below']) {
      _slides_admin_table_tree($data['below'], $form);
    }
  }

  return $form;
}

/**
 * Returns HTML for a slides administration form.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see slides_admin_table()
 * @ingroup themeable
 */
function theme_slides_admin_table($variables) {
  $form = $variables['form'];

  drupal_add_tabledrag('slides-outline', 'match', 'parent', 'slides-plid', 'slides-plid', 'slides-mlid', TRUE, MENU_MAX_DEPTH - 2);
  drupal_add_tabledrag('slides-outline', 'order', 'sibling', 'slides-weight');

  $header = array(t('Title'), t('Weight'), t('Parent'), array('data' => t('Operations'), 'colspan' => '3'));

  $rows = array();
  $destination = drupal_get_destination();
  $access = user_access('administer nodes');
  foreach (element_children($form) as $key) {
    $nid = $form[$key]['nid']['#value'];
    $href = $form[$key]['href']['#value'];

    // Add special classes to be used with tabledrag.js.
    $form[$key]['plid']['#attributes']['class'] = array('slides-plid');
    $form[$key]['mlid']['#attributes']['class'] = array('slides-mlid');
    $form[$key]['weight']['#attributes']['class'] = array('slides-weight');

    $data = array(
      theme('indentation', array('size' => $form[$key]['depth']['#value'] - 2)) . drupal_render($form[$key]['title']),
      drupal_render($form[$key]['weight']),
      drupal_render($form[$key]['plid']) . drupal_render($form[$key]['mlid']),
      l(t('view'), $href),
      $access ? l(t('edit'), 'node/' . $nid . '/edit', array('query' => $destination)) : '&nbsp;',
      $access ? l(t('delete'), 'node/' . $nid . '/delete', array('query' => $destination) )  : '&nbsp;',
    );
    $row = array('data' => $data);
    if (isset($form[$key]['#attributes'])) {
      $row = array_merge($row, $form[$key]['#attributes']);
    }
    $row['class'][] = 'draggable';
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'slides-outline')));
}
