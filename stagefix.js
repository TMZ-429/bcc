#!/usr/bin/env node
const fs = require('fs');

let stages = JSON.parse(fs.readFileSync("./stages.json").toString("utf8")).sampledata;

(async () => {
    for (let l = 0; l < stages.length; l++) {
        if (l == 4 || l == 9) { continue; }
        for (let i = 0; true; i++) {
            try {
                if (!stages[l][i][0]) {
                    console.log(stages[l][i]);
                    stages[l].splice(i, 1);
                }
            } catch (error) {
                break;
            }
        }
    }
})();

fs.truncate("./stages.json", () => {
    fs.writeFileSync("./stages.json", JSON.stringify({ "sampledata": stages }));
});