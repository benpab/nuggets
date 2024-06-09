# SCRUM Documentation

### Team VI 

## Scrum Roles
### Product Owner
- Professor Charles Palmer

### Scrum Master
- Veronica Yarovinsky

### Development Team
- **Cameron MacDonald**: Server
- **Benjamin Pable**: Client + Visibility
- **Kevin Luo**: Map
- **Oskar Magnusson**: Player

## Sprint 1 (May 13-17)
**Monday:**
- Project Board and Sprint Planning

**Tuesday:**
- Daily Scrum

**Wednesday:**
- Daily Scrum

** Middle Goals:**

- `DESIGN.md` complete
- `IMPLEMENTATION.md` drafted

**Friday:**
- Sprint Retrospective
- Planning for Implementation spec
- Plan division of labor
- Plan for testing

**Sunday:**
- Daily Scrum

**Goals:**
- `IMPLEMENTATION.md` complete, ready for submission
- Update documentation to track changes in design, implementation, or testing plan

## Sprint 2 (May 18-May 22)
**Saturday:**
- Sprint Retrospective + Review + Planning

**Sunday:**
- Daily Scrum

**Monday:**
- Daily Scrum

** Middle Goals:**

- **Server:** Initialize and receive messages from client, print them out to `stdout`.
- **Client:** Initialize and receive messages from server, print them out to `stdout`.
- **Player:** Initialize player and `movePlayer` functionality.
- **Map:** `Load map` and `Load Gold` work. `ConvertMap` and `deleteMap` behave appropriately.
- **Core Structures:** All struct types defined (`map`, `gamestate`, `player`)
- Update testing files to check new behaviors
- Extensive testing to ensure all code meets spec
- Update required documentation depending on changes

**Tuesday:**
- Sprint Retrospective + Review + Planning

**Wednesday:**
- Daily Scrum

**Goals:**
- **Server:** Behaves as should with the client without any errors. Send the correct message to client to output. Server quits (`gamestateDelete`) without any errors or memory leaks.
- **Client:** Outputs the correct information depending on messages from the server. Send the correct keypress back to the server.
- **Player:** Player visibility working. Player delete functionality behaves appropriately.
- **Map:** `Set` and `get location` functions working. `CopyMap` and `replaceGoldspot` functions change the map appropriately.
- Update testing files to check new behaviors
- Extensive testing to ensure all code meets spec
- Update required documentation depending on changes

## Sprint 3 (May 23-May 28)
**Thursday:**
- Daily Scrum, finalize each of our own individual modules

**Friday:**
- Daily Scrum, combine all modules together 

**Saturday:**
- Daily Scrum, write testing files, double check algorithm for visibility 

**Middle Goals:**

- Ensure no memory leaks in each of individual module
- **Server/Player:** Add sprinting functionality
- **Server/Player:** Handle invalid keypress appropriately
- **Map/Player:** All map functions are working properly
- Write tools to assist in testing
- Extensive testing to ensure all code meets spec
- Complete the required documentation
- Everyone has looked through each other's modules and is familiar with each other's code

**Sunday:**
- Daily Scrum, Sprint Retrospective, Review, Planning

**Monday:**
- Daily Scrum, play over game, Testing, Testing, Testing!!!

**Tuesday:**
- Daily Scrum, read over / finalize all documentation

**Goals:**
- Ensure game functionality is working 100%
- Ensure functionality aligns with requirement spec
- Review the Final submission checklist
- Clean up your repository
- Remove all unnecessary files (`.o` files, binary files, etc.)
- Touch up all documentation
- Touch up comments
- Regression test on the servers
- **Server:** Test all error and edge cases
- **Client:** Test all error and edge cases
- **Map:** Test all error and edge cases
- **Player:** Fix players crashing into each other bug. Test all error and edge cases
- Commit and push well before the deadline
- Prepare your presentation
