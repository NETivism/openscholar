<?php

/**
 * @file
 * Contains OsImporterNewsValidator
 */
class OsImporterNewsValidator extends OsImporterEntityValidateBase {

  public function setFieldsInfo() {
    $fields = parent::setFieldsInfo();

    $fields['field_news_date__start'] = array(
      'validators' => array(
        'isNotEmpty',
        'validateOsDate',
      ),
    );

    $fields['field_photo'] = array(
      'validators' => array(
        'validatorNewsPhoto',
      ),
    );

    return $fields;
  }

  /**
   * Validating the image is in 220X220.
   */
  public function validatorNewsPhoto($field_name, $value) {
    $this->validatorPhoto($field_name, $value, 250, 250);
  }

  public function isValidValue($field_name, $value, $type) {
    if ($type == 'field_item_image') {
      $value = array($value);
    }
    parent::isValidValue($field_name, $value, $type);
  }
}
