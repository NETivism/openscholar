Feature: User functionality testing.

  @api @last
  Scenario: Verify that user pages are inaccessible to anonymous users.
    Given I am not logged in
     Then I should be redirected in the following <cases>:
  #  | Request                    | Response Code | Final URL   |
     | users/admin                | 403           | users/admin |

  @api @last
  Scenario: User pages are accessible to the logging in user.
    Given I am logging in as "john"
     When I visit "/user"
     Then I should see "View"
      And I should see "Edit"

  @api @last
  Scenario: Adding a user to a vsite.
    Given I am logging in as "john"
     When I visit "john/cp/users/add"
      And I fill in "User" with "michelle"
      And I press "Add users"
     Then I should see "michelle has been added to the group John."

  @api @last
  Scenario: Enable custom roles and permissions in a VSite.
    Given I am logging in as "john"
      And I visit "john/cp/users/permissions"
     When I click "Edit roles and permissions"
      And I press "Confirm"
      And I visit "john/cp/users/permissions"
     Then I should see the button "Save permissions"

  @api @last
  Scenario: Create a custom role in a vsite.
    Given I am logging in as "john"
     When I visit "john/cp/users/roles"
      And I fill in "Name" with "New Custom Role"
      And I press "Add role"
      And I visit "john/cp/users/roles"
     Then I should see "New Custom Role"
      And I give the role "New Custom Role" in the group "john" the permission "Create Blog entry content"

  @api @last
  Scenario: Assign a custom role to a vsite member.
    Given I am logging in as "john"
     When I give the user "klark" the role "New Custom Role" in the group "john"
     Then I should verify that the user "klark" has a role of "New Custom Role" in the group "john"

  @api @last
  Scenario: Test node creation permissions of a custom role - check failure.
    Given I am logging in as "klark"
     When I go to "john/node/add/book"
     Then I should get a "403" HTTP response

  @api @last
  Scenario: Test node creation permissions of a custom role - check success.
    Given I am logging in as "klark"
     When I go to "john/node/add/blog"
     Then I should get a "200" HTTP response

  @api @last
  Scenario: Restore default roles and permissions in a VSite.
    Given I am logging in as "john"
      And I visit "john/cp/users/roles"
     When I click "Restore default roles and permissions"
      And I press "Confirm"
      And I visit "john/cp/users/roles"
     Then I should not see the button "Save permissions"

  @api @last
  Scenario: Test adding a new member by creating a new user on the site.
    Given I am logging in as "john"
      And I visit "john/cp/users/add"
     When I fill in "edit-name--2" with "Peter"
      And I fill in "edit-mail" with "peter@example.com"
      And I press "Create and add member"
     Then I should see "Peter has been added to the website: John"
      And I should verify that the user "Peter" has a role of "vsite member" in the group "john"

  @api @last
  Scenario: Test adding a new member by creating a new user on the site when
            using a shared domain.
    Given I am logging in as "admin"
      And I define "john" domain to "lincoln.local"
      And I am logging in as "john" in the domain "lincoln.local"
      And I visit "http://lincoln.local/john/cp/users/add"
     When I fill in "edit-name--2" with "Louis"
      And I fill in "edit-mail" with "louis@example.com"
      And I press "Create and add member"
     Then I should see "Louis has been added to the website: John"
      And I should verify that the user "Louis" has a role of "vsite member" in the group "john"
