<?php

/**
 * @file
 * Default theme implementation for printed version of slides outline.
 *
 * Available variables:
 * - $title: Top level node title.
 * - $head: Header tags.
 * - $language: Language code. e.g. "en" for english.
 * - $language_rtl: TRUE or FALSE depending on right to left language scripts.
 * - $base_url: URL to home page.
 * - $contents: Nodes within the current outline rendered through
 *   slides-node-export-html.tpl.php.
 *
 * @see template_preprocess_slides_export_html()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">
  <head>
    <title><?php print $title; ?></title>
    <?php print $head; ?>
    <base href="<?php print $base_url; ?>" />
	<?php $module_path = drupal_get_path('module', 'slides'); ?>
        <meta name="apple-mobile-web-app-capable" content="yes" />
        <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent" />
        
        <link href='http://fonts.googleapis.com/css?family=Lato:400,700,400italic,700italic' rel='stylesheet' type='text/css'>                                          
        
        <link rel="stylesheet" href="/drupal-7.10/<?php print $module_path; ?>/reveal/css/main.css">
        <link rel="stylesheet" href="/drupal-7.10/<?php print $module_path; ?>/reveal/css/theme/default.css" id="theme">

        <!-- For syntax highlighting -->
        <link rel="stylesheet" href="/drupal-7.10/<?php print $module_path; ?>/reveal/lib/css/zenburn.css">

        <script>
            // If the query includes 'print-pdf' we'll use the PDF print sheet
            document.write( '<link rel="stylesheet" href="/drupal-7.10/<?php print $module_path; ?>/reveal/css/print/' + ( window.location.search.match( /print-pdf/gi ) ? 'pdf' : 'paper' ) + '.css" type="text/css" media="print">' );
        </script>


  </head>
  <body>

        <div class="reveal">                                                                                                                                            

            <!-- Used to fade in a background when a specific slide state is reached -->
            <div class="state-background"></div>
                
            <!-- Any section element inside of this container is displayed as a slide -->
            <div class="slides">

			<?php
			/**
			 * The given node is /embedded to its absolute depth in a top level
			 * section/. For example, a child node with depth 2 in the hierarchy is
			 * contained in (otherwise empty) &lt;div&gt; elements corresponding to
			 * depth 0 and depth 1. This is intended to support WYSIWYG output - e.g.,
			 * level 3 sections always look like level 3 sections, no matter their
			 * depth relative to the node selected to be exported as printer-friendly
			 * HTML.
			 */
			$div_close = '';
			?>
			<?php for ($i = 1; $i < $depth; $i++): ?>
			  <div class="slide-<?php print $i; ?>">
			  <?php $div_close .= '</div>'; ?>
			<?php endfor; ?>
			<?php print $contents; ?>
			<?php print $div_close; ?>
			</div><!--end .slides-->
	

            <!-- The navigational controls UI -->
            <aside class="controls">
                <a class="left" href="#">&#x25C4;</a>
                <a class="right" href="#">&#x25BA;</a>
                <a class="up" href="#">&#x25B2;</a>
                <a class="down" href="#">&#x25BC;</a>
            </aside>

            <!-- Presentation progress bar -->
            <div class="progress"><span></span></div>
            
		</div><!--end .reveal-->

        <script src="/drupal-7.10/<?php print $module_path; ?>/reveal/lib/js/head.min.js"></script>
        <script src="/drupal-7.10/<?php print $module_path; ?>/reveal/js/reveal.min.js"></script>

        <script>
            
            // Full list of configuration options available here:
            // https://github.com/hakimel/reveal.js#configuration
            Reveal.initialize({
                controls: true,
                progress: true,
                history: true,
                
                theme: Reveal.getQueryHash().theme || 'default', // available themes are in /css/theme
                transition: Reveal.getQueryHash().transition || 'default', // default/cube/page/concave/linear(2d)

                // Optional libraries used to extend on reveal.js
                dependencies: [
                    { src: '/drupal-7.10/<?php print $module_path; ?>/reveal/lib/js/highlight.js', async: true, callback: function() { window.hljs.initHighlightingOnLoad(); } },
                    { src: '/drupal-7.10/<?php print $module_path; ?>/reveal/lib/js/classList.js', condition: function() { return !document.body.classList; } },
                    { src: '/drupal-7.10/<?php print $module_path; ?>/reveal/lib/js/showdown.js', condition: function() { return !!document.querySelector( '[data-markdown]' ); } },
                    { src: '/drupal-7.10/<?php print $module_path; ?>/reveal/lib/js/data-markdown.js', condition: function() { return !!document.querySelector( '[data-markdown]' ); } },
                    { src: '/socket.io/socket.io.js', async: true, condition: function() { return window.location.host === 'localhost:1947'; } },
                    { src: '/drupal-7.10/<?php print $module_path; ?>/reveal/plugin/speakernotes/client.js', async: true, condition: function() { return window.location.host === 'localhost:1947'; } },
                ]
            });
            
        </script>


  </body>
</html>
