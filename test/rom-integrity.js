// This test file is run using nes-test. 
// Check out the documentatation here: https://cppchriscpp.github.io/nes-test/
// The tool will automatically run the tests in every file in this directory, so feel free to spread them out!
const NesRomFile = require('nes-test').NesRomFile;

const romPath = '../rom/nes-starter-kit-example.nes';


describe('Simple Tests', () => {

    it('Validates that this is a valid NES rom', async () => {
        // Rom paths are relative to the test script
        romData = new NesRomFile(romPath);
        expect(romData.hasValidHeader()).toEqual(true);
    });
});