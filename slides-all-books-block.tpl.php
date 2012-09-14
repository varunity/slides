<?php

/**
 * @file
 * Default theme implementation for rendering slides outlines within a block.
 * This template is used only when the block is configured to "show block on
 * all pages" which presents Multiple independent slidess on all pages.
 *
 * Available variables:
 * - $slides_menus: Array of slides outlines keyed to the parent slides ID. Call
 *   render() on each to print it as an unordered list.
 */
?>
<?php foreach ($slides_menus as $slides_id => $menu): ?>
  <div id="slides-block-menu-<?php print $slides_id; ?>" class="slides-block-menu">
    <?php print render($menu); ?>
  </div>
<?php endforeach; ?>
