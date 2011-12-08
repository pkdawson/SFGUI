#include <SFGUI/ComboBox.hpp>
#include <SFGUI/Context.hpp>
#include <SFGUI/Engine.hpp>

#include <limits>

namespace sfg {

const ComboBox::IndexType ComboBox::NONE = std::numeric_limits<ComboBox::IndexType>::max();
static const sf::String EMPTY = "";

ComboBox::ComboBox() :
	Bin(),
	m_active( false ),
	m_active_item( NONE ),
	m_highlighted_item( NONE )
{
}

ComboBox::~ComboBox() {
}

ComboBox::Ptr ComboBox::Create() {
	ComboBox::Ptr ptr( new ComboBox );
	return ptr;
}

RenderQueue* ComboBox::InvalidateImpl() const {
	return Context::Get().GetEngine().CreateComboBoxDrawable( DynamicPointerCast<const ComboBox>( shared_from_this() ) );
}

ComboBox::IndexType ComboBox::GetSelectedItem() const {
	return m_active_item;
}

ComboBox::IndexType ComboBox::GetHighlightedItem() const {
	return m_highlighted_item;
}

void ComboBox::SelectItem( IndexType index ) {
	if( index >= m_entries.size() ) {
		return;
	}

	m_active_item = index;
	Invalidate();
}

void ComboBox::AppendItem( const sf::String& text ) {
	m_entries.push_back( text );
	RequestResize();
}

void ComboBox::InsertItem( IndexType index, const sf::String& text ) {
	m_entries.insert( m_entries.begin() + index, text );

	if( m_active_item != NONE && m_active_item >= index ) {
		++m_active_item;
	}

	RequestResize();
}

void ComboBox::PrependItem( const sf::String& text ) {
	m_entries.insert( m_entries.begin(), text );

	if( m_active_item != NONE ) {
		++m_active_item;
	}

	RequestResize();
}

void ComboBox::ChangeItem( IndexType index, const sf::String& text ) {
	if( index >= m_entries.size() ) {
		return;
	}

	m_entries[index] = text;
	RequestResize();
}

void ComboBox::RemoveItem( IndexType index ) {
	if( index >= m_entries.size() ) {
		return;
	}

	m_entries.erase( m_entries.begin() + index );

	// Make sure active item index keeps valid.
	if( m_active_item != NONE ) {
		if( m_active_item == index ) {
			m_active_item = NONE;
		}
		else if( m_active_item > index ) {
			m_active_item = m_entries.size() == 0 ? NONE : m_active_item - 1;
		}
	}

	RequestResize();
}

const sf::String& ComboBox::GetSelectedText() const {
	if( m_active_item == NONE ) {
		return EMPTY;
	}

	return m_entries[m_active_item];
}

ComboBox::IndexType ComboBox::GetItemCount() const {
	return m_entries.size();
}

const sf::String& ComboBox::GetItem( IndexType index ) const {
	if( index >= m_entries.size() ) {
		return EMPTY;
	}

	return m_entries[index];
}

bool ComboBox::IsPoppedUp() const {
	return m_active;
}

void ComboBox::HandleMouseEnter( int /*x*/, int /*y*/ ) {
	if( GetState() == NORMAL ) {
		SetState( PRELIGHT );
	}
}

void ComboBox::HandleMouseLeave( int /*x*/, int /*y*/ ) {
	if( GetState() == PRELIGHT ) {
		SetState( NORMAL );
	}
}

void ComboBox::HandleMouseMoveEvent( int /*x*/, int y ) {
	if( m_active ) {
		float padding( Context::Get().GetEngine().GetProperty<float>( "Padding", shared_from_this() ) );
		const std::string& font_name( Context::Get().GetEngine().GetProperty<std::string>( "FontName", shared_from_this() ) );
		unsigned int font_size( Context::Get().GetEngine().GetProperty<unsigned int>( "FontSize", shared_from_this() ) );
		const sf::Font& font( *Context::Get().GetEngine().GetResourceManager().GetFont( font_name ) );
		
		IndexType line_y = y;
		line_y -= static_cast<int>( GetAllocation().Top + GetAllocation().Height + GetBorderWidth() + padding );
		line_y /= static_cast<int>( Context::Get().GetEngine().GetLineHeight( font, font_size ) );
		
		if( line_y < GetItemCount() ) {
			if( line_y != m_highlighted_item ) {
				Invalidate();
			}
			m_highlighted_item = line_y;
		}
	}
}

void ComboBox::HandleMouseButtonEvent( sf::Mouse::Button button, bool press, int /*x*/, int /*y*/ ) {
	if( button == sf::Mouse::Left && IsMouseInWidget() ) {
		if( press ) {
			SetState( ACTIVE );
		}
		else {
			SetState( PRELIGHT );
		}
	}
	
	if( button == sf::Mouse::Left ) {
		if( m_active && !press ) {
			if( m_highlighted_item != NONE ) {
				m_active_item = m_highlighted_item;
				m_active = false;
				m_highlighted_item = NONE;

				OnSelect();
				Invalidate();
			}
			else {
				m_active = false;
				Invalidate();
				m_highlighted_item = -1;
			}
		}
		else if( !m_active && press ) {
			m_active = true;
			OnOpen();
			Invalidate();
		}
	}
	
	if( !IsMouseInWidget() ) {
		SetState( NORMAL );
		m_active = false;
		m_highlighted_item = -1;
		Invalidate();
		return;
	}
}

sf::Vector2f ComboBox::CalculateRequisition() {
	float padding( Context::Get().GetEngine().GetProperty<float>( "Padding", shared_from_this() ) );
	const std::string& font_name( Context::Get().GetEngine().GetProperty<std::string>( "FontName", shared_from_this() ) );
	unsigned int font_size( Context::Get().GetEngine().GetProperty<unsigned int>( "FontSize", shared_from_this() ) );
	const sf::Font& font( *Context::Get().GetEngine().GetResourceManager().GetFont( font_name ) );

	sf::Vector2f metrics( 0.f, 0.f );
	for ( IndexType item = 0; item < GetItemCount(); ++item ) {
		if( Context::Get().GetEngine().GetTextMetrics( m_entries.at( item ), font, font_size ).x > metrics.x ) {
			metrics = Context::Get().GetEngine().GetTextMetrics( m_entries.at( item ), font, font_size );
		}
	}
	metrics.y = Context::Get().GetEngine().GetLineHeight( font, font_size );

	sf::Vector2f requisition(
		metrics.x + 2 * GetBorderWidth() + 2 * padding + metrics.y,
		metrics.y + 2 * GetBorderWidth() + 2 * padding
	);

	return requisition;
}

const std::string& ComboBox::GetName() const {
	static const std::string name( "ComboBox" );
	return name;
}

}