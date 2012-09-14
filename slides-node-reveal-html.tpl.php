<?php

/**
 * @file
 * Default theme implementation for rendering a single node in a printer
 * friendly outline.
 *
 * @see slides-node-export-html.tpl.php
 * Where it is collected and printed out.
 *
 * Available variables:
 * - $depth: Depth of the current node inside the outline.
 * - $title: Node title.
 * - $content: Node content.
 * - $children: All the child nodes recursively rendered through this file.
 *
 * @see template_preprocess_slides_node_export_html()
 */
?>
<section>
  <h2 class="slide-heading"><?php print $title; ?></h2>
  <?php print $content; ?>
</section>
  <?php print $children; ?>
