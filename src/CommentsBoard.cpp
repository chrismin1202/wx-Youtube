//Copyright (C) 2011  Groza Cristian, N3
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



#include "CommentsBoard.hpp"

CommentsBoard::CommentsBoard(wxWindow* parent, EventManager* evt_man, wxWindowID id): wxPanel(parent), m_v_sizer(0),
    m_comments_pane(0)
{
    // create the event handler an bind it
    on_select = new OnVideoSelect(this);
    on_search = new OnVideoSearch(this);
    evt_man -> BindEvent(on_select);
    evt_man -> BindEvent(on_search);

    // create the layout
    m_v_sizer = new wxBoxSizer(wxVERTICAL);
    m_comments_pane = new CommentsPane(this);
    m_v_sizer -> Add(m_comments_pane, 4, wxEXPAND | wxALL, 0);
    SetSizerAndFit(m_v_sizer);
}

void CommentsBoard::DeleteAllComments()
{
    // delete the old comments, free the memory and clear the vector.
    std::vector<CommentInfo*>::iterator del = m_comments -> begin();    // create iterator
    for(del; del < m_comments -> end(); ++del)
    {
        delete *del;
        *del = 0;	// delete comment and set pointer to NULL
    }
    m_comments -> clear();	// clear the null pointers from the vector
}

void CommentsBoard::OnFeedFetched(wxCommandEvent& event)
{
    // display the comments
    m_comments_pane -> RefreshCommentList();
    event.Skip();
}

void CommentsBoard::FetchCommentsFeed()
{
    if(m_current_vid)
    {
        wxString* video_id = new wxString(m_current_vid -> getId().c_str(), wxConvUTF8); // get video id
        // we must create the wxString on the heap because it must still exist during the thread execution.
        SearchURL* comment_url = new SearchURL(VIDEO_COMMENTS_SEARCH, *video_id, 0, 0);
        XMLFeed* xml_feed = new XMLFeed(comment_url);

        // create callback object
        FetchCommentsCallback* callback = new FetchCommentsCallback(this);
        // create and run the fetcher thread
        FeedFetcherThread* feed_fetch_thread = new FeedFetcherThread(xml_feed, callback);
        feed_fetch_thread -> Create();
        // run the thread
        feed_fetch_thread -> Run();
    }
}

CommentsBoard::CommentsPane::CommentsPane(CommentsBoard* parent) : wxScrolledWindow(parent), m_v_sizer(0), m_parent(parent)
{
    // create the layout
    m_v_sizer = new wxBoxSizer(wxVERTICAL);
    m_comment_display = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                                       wxTE_AUTO_URL | wxTE_MULTILINE | wxTE_READONLY);

    m_v_sizer -> Add(m_comment_display, 1, wxEXPAND|wxALL, 0);
    SetSizerAndFit(m_v_sizer);
}

void CommentsBoard::CommentsPane::AddComment(const CommentInfo* comment)
{
    // Append the comment to the text ctrl
    wxString author = wxString(comment -> getAuthor().c_str(), wxConvUTF8);
    wxString content = wxString(comment -> getContent().c_str(), wxConvUTF8);
    wxString newline = wxString("\n\n", wxConvUTF8);

    m_comment_display -> Freeze();
    m_comment_display -> WriteText(newline+wxT("(")+author+wxT(") "));
    m_comment_display -> WriteText(newline+content);
    m_comment_display -> Thaw();
}

void CommentsBoard::CommentsPane::RefreshCommentList()
{
    // remove the old comment text from the text ctrl
    m_comment_display -> Clear();
    // loop through the comments vector and add each comment to the text ctrl
    std::vector<CommentInfo*>::const_iterator it = m_parent ->  m_comments -> begin();
    m_comment_display ->Freeze();
    for(it; it < m_parent -> m_comments -> end(); ++it)
    {
        wxString author = wxString((*it) -> getAuthor().c_str(), wxConvUTF8);
        wxString content = wxString((*it) -> getContent().c_str(), wxConvUTF8);
        wxString newline = wxString("\n\n", wxConvUTF8);
        m_comment_display -> WriteText(newline+wxT("[")+author+wxT("] ")+content);
    }
    m_comment_display -> Thaw();
}

void CommentsBoard::CommentsPane::ClearCommentDisplay()
{
    m_comment_display -> SetValue(wxT(""));
}

std::vector<CommentInfo*>* CommentsBoard::m_comments = new std::vector<CommentInfo*>();
VideoInfo* CommentsBoard::m_current_vid = 0;

BEGIN_EVENT_TABLE(CommentsBoard, wxPanel)
    EVT_COMMAND (ON_FEED_FETCHED, wxEVT_COMMAND_TEXT_UPDATED, CommentsBoard::OnFeedFetched) // fired when the feed
    // fetcher is done
END_EVENT_TABLE()
