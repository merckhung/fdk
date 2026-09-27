// Renders HTML pages produced by ansi2html.py to PNG files.
// Usage: node tools/screenshot.mjs in1.html out1.png [in2.html out2.png ...]
import { chromium } from 'playwright';

const args = process.argv.slice(2);
const browser = await chromium.launch();
const page = await browser.newPage({ deviceScaleFactor: 2 });
for (let i = 0; i < args.length; i += 2) {
  await page.goto('file://' + args[i]);
  await page.locator('body').screenshot({ path: args[i + 1] });
}
await browser.close();
