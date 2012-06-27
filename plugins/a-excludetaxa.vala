/**
 * This is an example processing rule.
 */
class AutoQIIME.Analyses.ExcludeTaxa : RuleProcessor {
	/*
	 * Is this a new kind of analysis or a new source of sequence data?
	 */
	public override RuleType get_ruletype() {
		return RuleType.ANALYSIS;
	}
	/*
	 * What is the XML tag associated with this rule. Each XML tag needs its own rule, though you must process child tags, if desired.
	 */
	public override unowned string get_name() {
		return "exclude-taxonomy";
	}
	/*
	 * When generating the Makefile, is there a secondary Makefile that should be included?
	 */
	public override unowned string ? get_include() {
		return null;
	}

	/**
	 * What version of AutoQIIME was this feature introduced in?
	 *
	 * You should set this to the current version of AutoQIIME when you develop a plugin and never change it.
	 */
	public override version introduced_version() {
		return version(1, 5);
	}
	/*
	 * Can this rule be included multiple times? Each tag must generate some non-mutually infering set of Make rules.
	 */
	public override bool is_only_once() {
		return true;
	}

	/*
	 * Called for each tag, passed as defintion, to add rules to the nascent Makefile using Output.
	 */
	public override bool process(Xml.Node *definition, Output output) {
		output.add_target("exclude_otus.list");
		var excludes = definition->get_prop("taxa");
		var excludeslist = excludes.split(",");
		output.add_rulef("OTU_EXCLUDE_FILE := exclude_otus.list\n");
		output.add_rulef("OTU_EXCLUDE = -e exclude_otus.list\n\n");
		output.add_rulef("exclude_otus.list: rdp_assigned_taxonomy/seq.fasta_rep_set_tax_assignments.txt\n");
		output.add_rulef("\t@echo Creating taxa exclusion list...\n");
		output.add_rulef("\tawk 'BEGIN { FS=\" \"; print \"#Taxa Exclusion List Generated by AutoQIIME\" > \"exclude_otus.list\"; } { if (");
		bool first = true;
		foreach (string item in excludeslist) {
			if (first) {
				output.add_rulef("$$0 ~ /%s/ ",item.strip());
				first = false;
			} else {
				output.add_rulef("|| $$0 ~ /%s/ ",item.strip());
			}
		}
		output.add_rulef(") { print $$1 > \"exclude_otus.list\"; } }' rdp_assigned_taxonomy/seq.fasta_rep_set_tax_assignments.txt\n\n");
		return true;
	}
}
