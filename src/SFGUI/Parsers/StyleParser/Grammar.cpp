#include <SFGUI/Parsers/StyleParser/Grammar.hpp>

namespace sfg {
namespace parser {
namespace style {

StyleGrammar::StyleGrammar() :
	ell::Parser<char>( &style_rule, &skipper_rule )
{
	flags.look_ahead = false;

	GrammarToken();
	GrammarSimpleSelector();
	GrammarSelector();
	GrammarStatement();

	style_rule = ( end_rule | ( +rule_rule )[ &StyleGrammar::PushStyle ] ) >> end_rule;
	style_rule.set_name( "Identifier or *" );
}

const std::vector<struct Rule>& StyleGrammar::GetStyle() const {
	return m_style;
}

void StyleGrammar::ClearResults() {
	m_style.clear();
}

}
}
}
