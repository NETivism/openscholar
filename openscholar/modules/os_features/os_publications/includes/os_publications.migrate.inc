<?php

class OsBiblioMigrateEntries extends BiblioMigrateEntries {

  public function __construct($arguments = array()) {
    parent::__construct($arguments);

    // Add group audience field to a Biblio entry.
    $this->query->innerJoin('og_membership', 'ogm', 'ogm.etid = b.nid');
    $this->query
      ->fields('ogm', array('gid'))
      ->condition('ogm.entity_type', 'node');

    $this->addFieldMapping(OG_AUDIENCE_FIELD, 'gid');
  }

  /**
   * Map field API fields from the node to the new Biblio entity.
   */
  public function prepare($entity, $row) {
    // Load the Biblio node to populate the appropriate fields
    // in the new created Biblio entity.
    $node = node_load($row->nid);

    // An array of fields to be added to the entry keyed by the new field name
    // and keyd by the old field name.
    $info = array(
      'biblio_image' => 'field_biblio_image',
      'biblio_pdf' => 'field_upload',
      'biblio_extra' => 'field_biblio_extra',
      'og_vocabulary' => 'og_vocabulary',
    );

    foreach ($info as $new_field => $old_field) {
      $entity->$new_field = $node->$old_field;
    }

    // Create a Biblio vocabulary.
    $this->createBiblioVocab($entity, $node);

    // @todo: add mapping to remaining fields:
    // redirect
  }

  /**
   * Apply needed changes to the new Biblio entity.
   */
  public function complete($entity, $row) {
    // Map the path property (update the URL alias table).
    // This is done here and not in the "prepare()" since we need the Biblio ID.
    $this->updatePathAlias($entity, $row);
  }

  /**
   * Create an OG vocab entity.
   *
   * If the vocabulary is not linked yet to the Biblio entity and its bundle,
   * add the vocab and the relation to the group.
   *
   * @param Biblio $biblio
   *    The saved Biblio.
   * @param $node
   *    The old Biblio node.
   */
  private function createBiblioVocab($biblio, $node) {
    if (empty($node->og_vocabulary)) {
      return;
    }

    $wrapper = entity_metadata_wrapper('biblio', $biblio);

    foreach ($wrapper->{OG_VOCAB_FIELD} as $terms) {
      $vid = $terms->vocabulary->getIdentifier();

      // Create the OG vocabulary and the relation to the group.
      if (!og_vocab_load_og_vocab($vid, 'biblio', $biblio->type)) {
        og_vocab_create_og_vocab($vid, 'biblio', $biblio->type)->save();
        og_vocab_relation_save($vid, 'node', $wrapper->{OG_AUDIENCE_FIELD}->get(0)->getIdentifier());
      }
    }
  }

  /**
   * Create the path (URL alias) for the new Biblio entry.
   *
   * @param Biblio $biblio
   *    The saved Biblio.
   * @param $row
   *    The old Biblio node.
   */
  private function updatePathAlias($biblio, $row) {
    $conditions = array(
      'source' => 'node/' . $row->nid,
      'language' => LANGUAGE_NONE,
    );

    if (!$path = path_load($conditions)) {
      return;
    }

    $path = path_load($conditions);
    $path['source'] = 'biblio/' . $biblio->bid;
    path_save($new_path);
  }
}