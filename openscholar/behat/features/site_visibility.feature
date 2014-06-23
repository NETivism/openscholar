Feature:
  Testing the visibility field.

  @api @first
  Scenario Outline: Define the site visibility field to "Anyone with the link"
                    and test that anonymous users can view the site.
     Given I visit <request-url>
      Then I should see <text>

  Examples:
    | request-url                     | text                                                |
    | "einstein"                      | "Einstein"                                          |
    | "einstein/blog"                 | "Mileva Marić"                                      |
    | "einstein/blog/mileva-marić"    | "Yesterday I met Mileva, what a nice girl :)."      |

  @api @first
  Scenario: Testing private vsite cannot be seen by anonymous users.
    Given I am logging in as "john"
     When I change privacy of the site "obama" to "Invite only during site creation. "
      And I click "Log out"
      And I go to "obama"
     Then I should get a "403" HTTP response

  @api @first
  Scenario: Testing private vsite cannot be seen by members from another vsite.
    Given I am logging in as "alexander"
      And I go to "obama"
     Then I should get a "403" HTTP response

  @api @first
  Scenario: Testing private vsite can be seen by support team members.
    Given I am logging in as "bill"
      And I go to "obama"
      And I should get a "200" HTTP response
      And I click "Support obama"
      And I should see "Are you sure you want to join the web site"
      And I press "Join"
     Then I should see "Your subscription request was sent."

  @api @first
  Scenario: Testing unsubscribing a support team member.
    Given I am logging in as "bill"
      And I go to "obama"
      And I should get a "200" HTTP response
      And I click "Unsubscribe obama"
      And I should see "Are you sure you want to unsubscribe from the group"
      And I press "Remove"
     Then I should see "Support obama"

  @api @first
  Scenario: Testing public vsite can be viewed by anonymous users.
    Given I am logging in as "john"
     When I change privacy of the site "obama" to "Public on the web. "
      And I click "Log out"
      And I visit "obama"
     Then I should get a "200" HTTP response

  @api @first
  Scenario: Testing public vsite can be seen by members from another vsite.
    Given I am logging in as "alexander"
      And I visit "obama"
     Then I should get a "200" HTTP response
