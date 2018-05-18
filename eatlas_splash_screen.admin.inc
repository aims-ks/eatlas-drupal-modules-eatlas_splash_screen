<?php

include_once('eatlas_splash_screen.constants.inc');


function _eatlas_splash_screen_overview() {
	$output = '<h3>' . t('eAtlas Splash Screen overview') . '</h3>';

	// Load all Splash Screen
	$splash_screen_array = eatlas_splash_screen_load_all();

	$header = array(t('Title'), array('data' => t('Operations'), 'colspan' => 2));
	$rows = array();
	foreach ($splash_screen_array as $splash_screen) {
		$block_id = EATLAS_SPLASH_SCREEN_BLOCK_PREFIX . $splash_screen->spid;
		$block = block_load('eatlas_splash_screen', $block_id);
		$rows[] = array(
			($block && property_exists($block, 'title')) ? $block->title : 'Untitled',
			l(
				t('edit'),
				"admin/structure/block/manage/eatlas_splash_screen/$block_id/configure",
				array(
					'query' => array('destination' => 'admin/config/eatlas/eatlas_splash_screen')
				)
			),
			l(
				t('delete'),
				"admin/config/eatlas/eatlas_splash_screen/" . $splash_screen->spid . "/delete",
				array(
					'query' => array('destination' => 'admin/config/eatlas/eatlas_splash_screen')
				)
			)
		);
	}

	$output .= theme('table', array(
			'header' => $header,
			'rows' => $rows,
			'empty' => t('No Splash Screen available.')
	));

	return $output;
}

function _eatlas_splash_screen_add_form($form, &$form_state, $spid = NULL) {
	// https://api.drupal.org/api/drupal/developer%21topics%21forms_api_reference.html/7
	$form = array();

	$splash_screen = NULL;
	if ($spid !== NULL) {
		$splash_screen = eatlas_splash_screen_load($spid);
	}

	$form['title'] = array(
		'#type' => 'textfield',
		'#title' => t('Block title'),
		'#description' => t('Splash Screen name, used in the block name.')
	);

	_eatlas_splash_screen_add_form_fields($form, $splash_screen);

	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
	);

	$form['spid'] = array(
		'#type' => 'hidden',
		'#value' => $spid
	);

	$form['actions']['delete'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#access' => _eatlas_splash_screen_admin_access(),
		'#weight' => 10,
	);

	return $form;
}

/**
 * Drupal magic: <form ID>_validate
 * Validate add / edit / delete Splash Screen form
 */
function _eatlas_splash_screen_add_form_validate($form, &$form_state) {}

/**
 * Drupal magic: <form ID>_submit
 * Submit add / edit / delete Splash Screen form
 */
function _eatlas_splash_screen_add_form_submit($form, &$form_state) {
	$form_state['redirect'] = 'admin/config/eatlas/eatlas_splash_screen';

	$spid = isset($form_state['values']['spid']) ? $form_state['values']['spid'] : NULL;
	$splash_screen = _get_edited_splash_screen($spid, $form_state['values']);
	$spid = eatlas_splash_screen_save($splash_screen);

	// Save the block title
	// NOTE: There is no API to do this...
	db_insert('block')->fields(array(
		'title' => $form_state['values']['title'],
		'module' => 'eatlas_splash_screen',
		'delta' => EATLAS_SPLASH_SCREEN_BLOCK_PREFIX . $spid,
		'pages' => ''
	))->execute();
}


/**
 * Display a personalised title for "Delete" Splash Screen block
 */
function _eatlas_splash_screen_delete_title($spid) {
	$block_id = EATLAS_SPLASH_SCREEN_BLOCK_PREFIX . $spid;
	$block = block_load('eatlas_splash_screen', $block_id);

	return t('Delete @splash_screen Splash Screen block', array(
		'@splash_screen' => ($block && property_exists($block, 'title')) ? $block->title : 'Untitled'
	));
}

/**
 * Called from "admin/config/eatlas/eatlas_splash_screen/%/delete"
 * Confirm deletion of a Splash Screen
 */
function _eatlas_splash_screen_form_confirm_delete($form, &$form_state, $spid) {
	$block_id = EATLAS_SPLASH_SCREEN_BLOCK_PREFIX . $spid;
	$block = block_load('eatlas_splash_screen', $block_id);

	// Always provide entity id in the same form key as in the entity edit form.
	$form['spid'] = array('#type' => 'value', '#value' => $spid);
	$form['delete'] = array('#type' => 'value', '#value' => TRUE);

	return confirm_form(
		$form,
		t('Are you sure you want to delete the Splash Screen %title?',
			array('%title' => ($block && property_exists($block, 'title')) ? $block->title : 'Untitled')
		),
		'admin/config/eatlas/eatlas_splash_screen',
		t('Deleting a Splash Screen will delete its block. ' .
			'This action cannot be undone.'),
		t('Delete'),
		t('Cancel')
	);
}

/**
 * Drupal magic: <form ID>_submit
 */
function _eatlas_splash_screen_form_confirm_delete_submit($form, &$form_state) {
	if (isset($form_state['values']['spid'])) {
		eatlas_splash_screen_delete($form_state['values']['spid']);
	}

	$form_state['redirect'] = 'admin/config/eatlas/eatlas_splash_screen';
	return;
}

?>
