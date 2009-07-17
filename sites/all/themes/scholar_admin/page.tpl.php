<?php
// $Id: page.tpl.php,v 1.10.2.4 2009/02/13 17:30:22 swortis Exp $
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>
<body class="<?php print $body_classes; ?>">
  <div id="container">
  	 <div id="top">
  	   <?php print $scholarcp_toolbar;?>
  	 </div>
     
     <div id="main-content">
		<div id="header">
			
			<h1><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home">
			  <?php print $site_name; ?></a></h1>
			
			<div id="nav" class="clear-block">
				<?php print $scholarcp_left;?>
			</div>
			
			<?php if (!empty($header) || !empty($breadcrumb)){ ?>
			<div id="header-region">
				<?php print $header; ?>
			</div> <!-- /header-region -->
			<?php }; ?>
			
			<?php print $search_box; ?>
		</div> <!-- /header -->
		
		
			
			<div id="content">
			  <?php if (!empty($title)){ ?>
				<h2 class="title"><?php print $title; ?></h2>
			  <?php }; ?>
			  <?php if (!empty($tabs)){ ?>
				<div class="tabs"><?php print $tabs; ?></div>
			  <?php }; ?>
			  <?php print $help; ?>
			  <?php print $messages; ?>
			  <?php print $content; ?>
			  <?php if (!empty($feed_icons)){ ?>
				<div class="feed-icons"><?php print $feed_icons; ?></div>
			  <?php }; ?>
			</div> <!-- /content -->
			
			<?php if (!empty($content_bottom)){ ?>
			  <div id="content-bottom"><?php print $content_bottom; ?></div>
			<?php }; ?>

		</div> <!-- / main-content -->

      <div id="footer">
        <?php if ($footer_message){ ?>
          	<div id="footer-message"><?php print $footer_message; ?></div>
        <?php }; ?>
        <?php if ($footer){ ?>
        	<?php print $footer; ?>
        <?php }; ?>
        <p id="copy">&copy; <?php echo date(Y);?></p>
      </div> <!-- /#footer -->
	</div> <!-- /container --> 
  <?php if ($closure_region){ ?>
    <div id="closure-blocks"><?php print $closure_region; ?></div>
  <?php }; ?>

  <?php print $closure; ?>
    </div> <!-- /container -->  
</body>
</html>