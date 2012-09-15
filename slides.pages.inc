<?php

/**
 * @file
 * User page callbacks for the slides module.
 */

/**
 * Menu callback; prints a listing of all slidess.
 */
function slides_render() {
  $slides_list = array();
  foreach (slides_get_slidess() as $slides) {
    $slides_list[] = l($slides['title'], $slides['href'], $slides['options']);
  }

  return theme('item_list', array('items' => $slides_list));
}

/**
 * Menu callback; Generates various representation of a slides page and its children.
 *
 * The function delegates the generation of output to helper functions.
 * The function name is derived by prepending 'slides_reveal_' to the
 * given output type. So, e.g., a type of 'html' results in a call to
 * the function slides_reveal_html().
 *
 * @param $type
 *   A string encoding the type of output requested. The following
 *   types are currently supported in slides module:
 *
 *   - html: HTML (printer friendly output)
 *
 *   Other types may be supported in contributed modules.
 * @param $nid
 *   An integer representing the node id (nid) of the node to reveal
 * @return
 *   A string representing the node and its children in the slides hierarchy
 *   in a format determined by the $type parameter.
 */
function slides_reveal($type, $nid) {
  $type = drupal_strtolower($type);

  $reveal_function = 'slides_reveal_' . $type;

  if (function_exists($reveal_function)) {
    print call_user_func($reveal_function, $nid);
  }
  else {
    drupal_set_message(t('Unknown reveal format.'));
    drupal_not_found();
  }
}

/**
 * This function is called by slides_reveal() to generate HTML for reveal.
 *
 * The given node is /embedded to its absolute depth in a top level
 * section/. For example, a child node with depth 2 in the hierarchy
 * is contained in (otherwise empty) &lt;div&gt; elements
 * corresponding to depth 0 and depth 1. This is intended to support
 * WYSIWYG output - e.g., level 3 sections always look like level 3
 * sections, no matter their depth relative to the node selected to be
 * revealed as printer-friendly HTML.
 *
 * @param $nid
 *   An integer representing the node id (nid) of the node to reveal.
 * @return
 *   A string containing HTML representing the node and its children in
 *   the slides hierarchy.
 */
function slides_reveal_html($nid) {
  if (user_access('access printer-friendly version')) {
    $reveal_data = array();
    $node = node_load($nid);
    if (isset($node->slides)) {
      $tree = slides_menu_subtree_data($node->slides);
      $contents = slides_reveal_traverse($tree, 'slides_node_reveal');
      return theme('slides_reveal_html', array('title' => $node->title, 'contents' => $contents, 'depth' => $node->slides['depth']));
    }
    else {
      drupal_not_found();
    }
  }
  else {
    drupal_access_denied();
  }
}
/**
 * Menu callback; Generates various representation of a slides page and its children.
 *
 * The function delegates the generation of output to helper functions.
 * The function name is derived by prepending 'slides_export_' to the
 * given output type. So, e.g., a type of 'html' results in a call to
 * the function slides_export_html().
 *
 * @param $type
 *   A string encoding the type of output requested. The following
 *   types are currently supported in slides module:
 *
 *   - html: HTML (printer friendly output)
 *
 *   Other types may be supported in contributed modules.
 * @param $nid
 *   An integer representing the node id (nid) of the node to export
 * @return
 *   A string representing the node and its children in the slides hierarchy
 *   in a format determined by the $type parameter.
 */
function slides_export($type, $nid) {
  $type = drupal_strtolower($type);

  $export_function = 'slides_export_' . $type;

  if (function_exists($export_function)) {
    print call_user_func($export_function, $nid);
  }
  else {
    drupal_set_message(t('Unknown export format.'));
    drupal_not_found();
  }
}

/**
 * This function is called by slides_export() to generate HTML for export.
 *
 * The given node is /embedded to its absolute depth in a top level
 * section/. For example, a child node with depth 2 in the hierarchy
 * is contained in (otherwise empty) &lt;div&gt; elements
 * corresponding to depth 0 and depth 1. This is intended to support
 * WYSIWYG output - e.g., level 3 sections always look like level 3
 * sections, no matter their depth relative to the node selected to be
 * exported as printer-friendly HTML.
 *
 * @param $nid
 *   An integer representing the node id (nid) of the node to export.
 * @return
 *   A string containing HTML representing the node and its children in
 *   the slides hierarchy.
 */
function slides_export_html($nid) {
  if (user_access('access printer-friendly version')) {
    $export_data = array();
    $node = node_load($nid);
    if (isset($node->slides)) {
      $tree = slides_menu_subtree_data($node->slides);
      $contents = slides_export_traverse($tree, 'slides_node_export');
      return theme('slides_export_html', array('title' => $node->title, 'contents' => $contents, 'depth' => $node->slides['depth']));
    }
    else {
      drupal_not_found();
    }
  }
  else {
    drupal_access_denied();
  }
}

/**
 * Menu callback; show the outline form for a single node.
 */
function slides_outline($node) {
  drupal_set_title($node->title);
  return drupal_get_form('slides_outline_form', $node);
}

/**
 * Build the form to handle all slides outline operations via the outline tab.
 *
 * @see slides_outline_form_submit()
 * @see slides_remove_button_submit()
 *
 * @ingroup forms
 */
function slides_outline_form($form, &$form_state, $node) {
  if (!isset($node->slides)) {
    // The node is not part of any slides yet - set default options.
    $node->slides = _slides_link_defaults($node->nid);
  }
  else {
    $node->slides['original_bid'] = $node->slides['bid'];
  }

  // Find the depth limit for the parent select.
  if (!isset($node->slides['parent_depth_limit'])) {
    $node->slides['parent_depth_limit'] = _slides_parent_depth_limit($node->slides);
  }
  $form['#node'] = $node;
  $form['#id'] = 'slides-outline';
  _slides_add_form_elements($form, $form_state, $node);

  $form['slides']['#collapsible'] = FALSE;

  $form['update'] = array(
    '#type' => 'submit',
    '#value' => $node->slides['original_bid'] ? t('Update slides outline') : t('Add to slides outline'),
    '#weight' => 15,
  );

  $form['remove'] = array(
    '#type' => 'submit',
    '#value' => t('Remove from slides outline'),
    '#access' => $node->nid != $node->slides['bid'] && $node->slides['bid'],
    '#weight' => 20,
    '#submit' => array('slides_remove_button_submit'),
  );

  return $form;
}

/**
 * Button submit function to redirect to removal confirm form.
 *
 * @see slides_outline_form()
 */
function slides_remove_button_submit($form, &$form_state) {
  $form_state['redirect'] = 'node/' . $form['#node']->nid . '/outline/remove';
}

/**
 * Handles slides outline form submissions from the outline tab.
 *
 * @see slides_outline_form()
 */
function slides_outline_form_submit($form, &$form_state) {
  $node = $form['#node'];
  $form_state['redirect'] = "node/" . $node->nid;
  $slides_link = $form_state['values']['slides'];
  if (!$slides_link['bid']) {
    drupal_set_message(t('No changes were made'));

    return;
  }

  $slides_link['menu_name'] = slides_menu_name($slides_link['bid']);
  $node->slides = $slides_link;
  if (_slides_update_outline($node)) {
    if ($node->slides['parent_mismatch']) {
      // This will usually only happen when JS is disabled.
      drupal_set_message(t('The post has been added to the selected slides. You may now position it relative to other pages.'));
      $form_state['redirect'] = "node/" . $node->nid . "/outline";
    }
    else {
      drupal_set_message(t('The slides outline has been updated.'));
    }
  }
  else {
    drupal_set_message(t('There was an error adding the post to the slides.'), 'error');
  }
}

/**
 * Menu callback; builds a form to confirm removal of a node from the slides.
 *
 * @see slides_remove_form_submit()
 *
 * @ingroup forms
 */
function slides_remove_form($form, &$form_state, $node) {
  $form['#node'] = $node;
  $title = array('%title' => $node->title);

  if ($node->slides['has_children']) {
    $description = t('%title has associated child pages, which will be relocated automatically to maintain their connection to the slides. To recreate the hierarchy (as it was before removing this page), %title may be added again using the Outline tab, and each of its former child pages will need to be relocated manually.', $title);
  }
  else {
    $description = t('%title may be added to hierarchy again using the Outline tab.', $title);
  }

  return confirm_form($form, t('Are you sure you want to remove %title from the slides hierarchy?', $title), 'node/' . $node->nid, $description, t('Remove'));
}

/**
 * Confirm form submit function to remove a node from the slides.
 *
 * @see slides_remove_form()
 */
function slides_remove_form_submit($form, &$form_state) {
  $node = $form['#node'];
  if ($node->nid != $node->slides['bid']) {
    // Only allowed when this is not a slides (top-level page).
    menu_link_delete($node->slides['mlid']);
    db_delete('slides')
      ->condition('nid', $node->nid)
      ->execute();
    drupal_set_message(t('The post has been removed from the slides.'));
  }
  $form_state['redirect'] = 'node/' . $node->nid;
}
