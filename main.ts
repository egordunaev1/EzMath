import { App, Editor, MarkdownView, Modal, Notice, Plugin, PluginSettingTab, Setting, Vault, FileSystemAdapter, PluginManifest } from "obsidian";
const bindings = require("bindings")
const fs = require("fs")
const path = require("path")

export default class EzMathPlugin extends Plugin {
	addon = null;

	get_global_path() {
		// Find absolute path to vault
		var global_path;
		if (this.app.vault.adapter instanceof FileSystemAdapter) {
			global_path = this.app.vault.adapter.getBasePath();
		} else {
			throw new Error("Unable to get path to vault");
		}

		return path.join(global_path, ".obsidian/plugins/EzMath");
	}

	find_addon(name: string) {
		const global_path = this.get_global_path();

		const buildDirs = [
			"build/Release/",
			"build/",
			"./"
		];

		var buildDir = "";
		for (var i = 0; i < buildDirs.length; ++i) {
			var tryPath = path.join(global_path, buildDir, buildDirs[i]);
			if (fs.existsSync(tryPath)) {
				return bindings({
					bindings: name,
					build: buildDir,
					module_root: global_path
				});
			}
		}

		throw new Error("Unable to find precompiled addon");
	}

	async onload() {
		this.addon = this.find_addon("ezmath.node");
		this.addCommand({
			id: "simplify",
			name: "simplify",
			editorCallback: (editor: Editor, view: MarkdownView) => {
				let latex = editor.getSelection();
				let res = this.addon.simplify(latex);
				editor.replaceSelection(res);
			}
		});
	}
}