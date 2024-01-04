INSTALLATION:

# First, use the makefile to compile the program & set up the directory the program relies on in /usr/local/share
```
# make
```
# Lastly, install

```
# make install
```

# Now, you'll be able to run the program with the command 'bcc'

I should also note that at the moment, the "**Extras**" and "**Catamin Stages**" are not accessible in the stage menu (Selecting them will crash the program).
This is explained in the extremely choice words I have for the MyGamatoto developers in the comments of install_db.js

You may want to update the program with the following commands whenever MyGamatoto updates their database (I'll try to upload up to date versions when they come out, but here's how to manually update anyways)

***Fair warning: Updating enemies takes upwards of an hour and updating stages takes upwards of 8 hours***

# First, if you want to manually update the databases download the latest version of the database with:
```
$ node install_db.js
```
# Then, run the following command until it does NOT print any output
```
$ node stagefix.js
```
