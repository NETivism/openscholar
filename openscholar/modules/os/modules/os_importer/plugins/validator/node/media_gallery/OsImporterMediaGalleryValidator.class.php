<?php

/**
 * @file
 * Contains OsImporterMediaGalleryValidator
 */
class OsImporterMediaGalleryValidator extends OsImporterEntityValidateBase {

  public function setFieldsInfo() {
    $fields = parent::setFieldsInfo();

    $fields['media_gallery_rows'] = $fields['media_gallery_columns'] = array(
      'validators' => array(
        'validateRowsColumns'
      ),
    );

    return $fields;
  }

  /**
   * The rows and columns should be positive.
   */
  public function validateRowsColumns($field_name, $value) {
    if (empty($value) || $value < 1) {
      $params['@value'] = $value;
      $this->setError($field_name, 'The field @field positive. The given value is: @value.', $params);
    }
  }
}
