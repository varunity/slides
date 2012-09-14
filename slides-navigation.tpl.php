<?php

/**
 * @file
 * Default theme implementation to navigate slidess. Presented under nodes that
 * are a part of slides outlines.
 *
 * Available variables:
 * - $tree: The immediate children of the current node rendered as an
 *   unordered list.
 * - $current_depth: Depth of the current node within the slides outline.
 *   Provided for context.
 * - $prev_url: URL to the previous node.
 * - $prev_title: Title of the previous node.
 * - $parent_url: URL to the parent node.
 * - $parent_title: Title of the parent node. Not printed by default. Provided
 *   as an option.
 * - $next_url: URL to the next node.
 * - $next_title: Title of the next node.
 * - $has_links: Flags TRUE whenever the previous, parent or next data has a
 *   value.
 * - $slides_id: The slides ID of the current outline being viewed. Same as the
 *   node ID containing the entire outline. Provided for context.
 * - $slides_url: The slides/node URL of the current outline being viewed.
 *   Provided as an option. Not used by default.
 * - $slides_title: The slides/node title of the current outline being viewed.
 *   Provided as an option. Not used by default.
 *
 * @see template_preprocess_slides_navigation()
 */
?>
<?php if ($tree || $has_links): ?>
  <div id="slides-navigation-<?php print $slides_id; ?>" class="slides-navigation">
    <?php print $tree; ?>

    <?php if ($has_links): ?>
    <div class="page-links clearfix">
      <?php if ($prev_url): ?>
        <a href="<?php print $prev_url; ?>" class="page-previous" title="<?php print t('Go to previous page'); ?>"><?php print t('‹ ') . $prev_title; ?></a>
      <?php endif; ?>
      <?php if ($parent_url): ?>
        <a href="<?php print $parent_url; ?>" class="page-up" title="<?php print t('Go to parent page'); ?>"><?php print t('up'); ?></a>
      <?php endif; ?>
      <?php if ($next_url): ?>
        <a href="<?php print $next_url; ?>" class="page-next" title="<?php print t('Go to next page'); ?>"><?php print $next_title . t(' ›'); ?></a>
      <?php endif; ?>
    </div>
    <?php endif; ?>

  </div>
<?php endif; ?>
