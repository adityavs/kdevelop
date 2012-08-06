{% load kdev_filters %}
/*

 {{ license|lines_prepend:" * " }}
 */

#include "{{ output_file_header }}"
#include <QtTest>


void initTestCase()
{
    // Called before the first testfunction is executed
}


void cleanupTestCase()
{
    // Called after the last testfunction was executed
}


void init()
{
    // Called before each testfunction is executed
}


void cleanup()
{
    // Called after every testfunction
}


{% for case in testCases %}

void {{ case }}()
{


}

{% endfor %}

QTEST_MAIN({{ name }});
#include {{ output_file_header|cut:".h" }}.moc