<?php

/**
 * @file
 * Contains OsImporterClassValidator
 */
class OsImporterClassValidator extends OsImporterEntityValidateBase {

  public function setFieldsInfo() {
    $fields = parent::setFieldsInfo();

    $fields['field_semester'] = array(
      'validators' => array(
        'isNotEmpty',
        'validationSemester',
      ),
    );

    $fields['field_offered_year__start'] = array(
      'validators' => array(
        'validateOfferedYear',
      ),
    );

    return $fields;
  }

  /**
   * Validate the semester field.
   */
  function validationSemester($field_name_name, $value) {
    $info = field_info_field($field_name_name);

    // Validate the semester.
    $allowed_values = $info['settings']['allowed_values'] + array('N/A' => 'N/A');
    if (!in_array($value, $allowed_values)) {
      $params = array(
        '@allowed-values' => implode(', ', $allowed_values),
        '@value' => $value,
      );

      $this->setError($field_name_name, 'The given value(@value) is not validate value for the semester field and should be one of @allowed-values', $params);
    }
  }

  /**
   * Preprocess the offered year and preprocess the form on the way.
   */
  function validateOfferedYear($field_name, $value) {
    $value = reset($value);
    if (!is_numeric($value) || (is_numeric($value) && $value > 9999)) {
      $params = array(
        '@value' => $value,
      );

      $this->setError($field_name, 'The value for the year field is not valid value(@value). The value should be a year.', $params);
    }
  }
}
