#!/usr/bin/python
# http://toys.jacobian.org/presentations/2005/appscript/
# http://pypi.python.org/pypi/appscript/
from appscript import *

ab = app('Address Book.app')
people = ab.people	# (its.emails != [])
for person in people.get():
    emails = ', '.join(person.emails.value.get())
    print "%s: %s" % (person.name.get(), emails)
    
