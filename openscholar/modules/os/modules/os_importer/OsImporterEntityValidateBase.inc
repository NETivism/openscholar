<?php
/**
 * @file
 * contains OsImporterEntityValidateBase
 */

/**
 * Extending the entity validator class for plugins that just need to validate
 * the title and nothing more.
 */
class OsImporterEntityValidateBase extends EntityValidateBase {
  public function getFieldsInfo() {
    $fields = parent::getFieldsInfo();

    // Add all fields.
    foreach (field_info_instances($this->entityType, $this->bundle) as $field_name => $info) {
      if ($info['required']) {
        $fields[$field_name]['validators'][] = 'isNotEmpty';
      }
    }

    // Remove all validators for entityreference fields.
    foreach ($fields as $field_name => $info) {
      $field_info = field_info_field($field_name);
      $type = $field_info['type'];
      if ($type == 'entityreference') {
        unset($fields[$field_name]['validators']);
      }
    }

    return $fields;
  }

  /**
   * Verify the start is occurring before the end date.
   */
  public function validateOsDate($field_name, $value) {
    $value = reset($value);
    // Validate the date format for the start and end date.
    $date = DateTime::createFromFormat('M j Y', $value);

    if ($date && $date->format('M j Y') == $value) {
      return;
    }

    $params = array(
      '@date' => $value,
      '@format' => date('M j Y'),
    );
    $this->setError($field_name, 'The date, @date, is not valid. The date should be in a format similar to @format', $params);
  }

  /**
   * Validating the url field is not empty. We can't use the isNotEmpty since
   * the field is array with keys.
   */
  public function validatorUrlNotEmpty($field_name, $value) {
    if (empty($value['url'])) {
      $params = array(
        '@field_name' => $field_name,
      );
      $this->setError($field_name, t('The url on @field_name is empty. Please supply a URL.', $params));
    }
  }

  /**
   * Validating the image minimum size.
   */
  public function validatorPhoto($field_name, $value, $min_width, $min_height) {
    list($width, $height) = getimagesize($value);

    if ($width < $min_width || $height < $min_height) {
      $params = array(
        '@width' => $width,
        '@height' => $height,
        '@min_width' => $min_width,
        '@min_height' => $min_height,
      );
      $this->setError($field_name, 'The size of the image need to be at least @min_widthX@min_height. The given image is @widthX@height', $params);
    }
  }
}
