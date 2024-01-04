#!/usr/bin/env node
const axios = require('axios');
const fs = require('fs');

databases = [
    ["https://onestoppress.com/api/allcats", "database"],
    ["https://onestoppress.com/api/allenemies", "enemies"],
    ["https://onestoppress.com/api/allstages", "stages"]
];

console.log("Downloading databases from MyGamatoto's provider");

(async () => {
    for (let database of databases) {
        console.log("Downloading contents of " + database[0])
        await axios({
            url: database[0],
            method: "GET"
        }).then(response => {
            let path = "./" + database[1] + ".json";
            fs.truncate(path, 0, () => {
                fs.writeFileSync(path, JSON.stringify(response.data));
            });
        }).catch(() => {
            console.log("Unknown error occurred");
        });
    }
    //Whoever assembled these databases belongs in an assembly line in a factory, earning minimum wage and living in a 1 bedroom studio apartment with a communal bathroom across the hall
    //Also I hope the apartment is a city away from the apartment so they have to deal with gruelling levels of morning rush hour and gas money cuts deeply into their nonexistent paycheck
    console.log("Parsing & fixing enemies.json");
    let enemies = JSON.parse(fs.readFileSync("./enemies.json")
     .toString('utf8')
     .replaceAll("enemy_name_en", "name")
     .replaceAll("enemy_no", "key")
    );
    let length = enemies.sampledata.length;
    for (let i = 0; i < length; i++) {
        let enemy = enemies.sampledata[i];
        console.log(`Fixing ${i}/${length}`);
        /*WHY IS THIS A THING? WHY NOT JUST HAVE ALL THESE STATS IN THE ENEMIES DATABASE LIKE WITH THE CATS??
        Also whoever made the types a series of booleans like {red: true, zombie: true, floating: false}
        FOR ALL 12 TYPES (Keep in mind, new types get added)
        instead of a simple array like {types: ["Red", "Black", "Angel"]} and just leave out the types that aren't applicable
        If it turns out they have a better paying computer science job than I get with that level of incompetence then I'm just gonna go to business school
        */
        await axios({
            url: "https://onestoppress.com/api/singleenemy",
            method: "POST",
            data: { "compare": enemy.name }
        }).then(response => {
            let stats = response.data.sampledata;
            enemies.sampledata[i] = {
                "key": stats.enemy_no,
                "name": enemy.name,
                "rarity": enemy.type,
                "health": stats.hp,
                "damage": stats.atk,
                "target": (parseInt(stats.att_type) ? "Area" : "Single"),
                "kb": stats.kb,
                "dps": stats.dps,
                "ability": stats.special_att_desc,
                "range": stats.range,
                "cost": stats.money,
                "speed": stats.speed,
                "tba": stats.real_tba
            };
        });
    }
    fs.truncate("./enemies.json", () => {
        fs.writeFileSync("./enemies.json", JSON.stringify(enemies));
    });
    //Prediction: I'm going to blow a freaking fuse in these comments when I find out what formatting the stages will be like
    //Update: 50 lines into the algorithm for fixing the stages database -- no further comment
    //IT'S GONNA TAKE 3 DAYS TO COMPILE THIS DATABASE?!
    //Okay I got the math a bit wrong -- it'll only take somewhere around 8-10 hours
    console.log("Parsing & fixing stages");
    var finishedStages = [], finishedCategories = [], newChapters = [];
    let stages = JSON.parse(fs.readFileSync("./stages.json").toString("utf8")).sampledata;
    /*Why aren't the stages sorted like:
    "Stories of Legends": [
        {
            "chapter": "Body & Soul",
            "stages": [
                "Sunset's Howl": {
                    ...
                },
                ...
            ]
        }
    ],
    "Into The Future": [
        {
            "chapter": "Time Enough For Cats"
            "stages": [
                ...(You get the point)
            ]
        }
    ]
    Instead it's a massive array containing every stage in the game except it only has enemies, magnification and a STRING saying which stage group it belongs to (ITF, SoL, etc)
    WHY IS THIS A THING? This is like having to pack away sweaters, pants and socks and putting individual clothing items into little boxes labelled "part of pants"
    Instead of just packing a box of all pants, a box of all sweaters & a box of all socks. 
    */
    for (let i = 0; i < stages.length; i++) {
        let stage = stages[i];
        console.log(`Fixing ${i}/${stages.length}`);
        if (finishedCategories.includes(stage.category)) {
            stages.splice(i, 1);
            continue;
        }
        newChapters[i] = stages.filter(staging => staging.category == stages[i].category);
        finishedCategories.push(stages[i].category);
        for (let l = 0; l < newChapters[i].length; l++) {
            let chap = newChapters[i][l].chapter;
            if (finishedStages.includes(chap)) {
                newChapters[i].splice(l, 1);
                continue;
            }
            console.log(`[Subprocess #${i}]: Fixing ${l}/${newChapters[i].length} | (${(l / newChapters[i].length) * 100}%)`);
            finishedStages.push(chap);
            newChapters[i][l] = newChapters[i].filter(staging => staging.chapter == newChapters[i][l].chapter);
            for (let k = 0; k < newChapters[i][l].length; k++) {
                await axios({
                    url: "https://onestoppress.com/api/singlestage",
                    method: "POST",
                    data: { "stagenum": newChapters[i][l][k].stageid }
                }).then(response => {
                    let stageData = response.data.sampledata;
                    newChapters[i][l][k] = stageData;
                }).catch(() => {
                    k--;
                })
            }
        }
    }
    fs.truncate("./stages.json", () => {
        fs.writeFileSync("./stages.json", 
            JSON.stringify({ "sampledata": newChapters })
             .replaceAll(`":null`, `":"none"`)
             .replaceAll("null", "")
        );
    });
})();