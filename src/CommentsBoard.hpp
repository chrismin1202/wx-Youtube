#ifndef COMMENTS_BOARD_H
#define COMMENTS_BOARD_H

#include <wx/wx.h>
#include <vector>
#include <string>
#include "EventManager.hpp"
#include "Events.hpp"
#include "XMLFeed.hpp"
#include "SearchURL.hpp"
#include "Enums.hpp"
#include "Parser.hpp"
#include "VideoInfo.hpp"
#include "CommentInfo.hpp"
#include "FeedFetcherThread.hpp"
#include "DownloadCallback.hpp"
#include <wx/scrolwin.h>


class CommentsPane ;
class CommentsBoard : public wxPanel
{
    /*Dispalys and manages video comments.*/
public:
    CommentsBoard(wxWindow* parent, EventManager* evt_man, wxWindowID id = wxID_ANY);

    void DeleteAllComments();

    friend class FetchCommentsCallback;
    friend class OnVideoSelect;
    friend class CommentsPane;

protected:

    static const int ON_FEED_FETCHED = 100000;

    wxBoxSizer* m_v_sizer;
    CommentsPane* m_comments_pane;


    wxTextCtrl* m_comment_txt;
    static std::vector<CommentInfo*>* m_comments;
    static VideoInfo* m_current_vid;

    void OnFeedFetched(wxCommandEvent& event); 	// called when the comments feed has been fetched and parsed.
    void FetchCommentsFeed();   // starts a thread that fetches the comments

    class FetchCommentsCallback: public FeedFetcherCallback
    {
    public:
	FetchCommentsCallback(CommentsBoard* parent): m_parent(parent){}
	virtual void operator()(rapidxml::xml_document<>* feed, int exit_code)
	    {
		// create event and post it to parent

		m_parent -> DeleteAllComments();

		if(exit_code)
		{ // there was an error when curl fetched the feed.
		    delete feed ;
		    return;
		}
		// parse and populate received feed

		Parser::parseCommentsFeed(CommentsBoard::m_comments, feed);

		delete feed;		     // free the memorry occupied by the feed

		// Notify the parent via an event
		wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, CommentsBoard::ON_FEED_FETCHED);
		event.SetInt(exit_code);
		m_parent -> GetEventHandler() -> AddPendingEvent(event);
		return;

	    }
    private:
	CommentsBoard* m_parent;
    };


    class OnVideoSelect: public VideoSelectObjectFunction
    {
    public:
	OnVideoSelect(CommentsBoard* parent) : m_parent(parent) {}
	virtual void operator()(VideoSelectEvent* event)
	    {
		CommentsBoard::m_current_vid = event -> GetVideoInfo();
		m_parent -> FetchCommentsFeed();

	    }
    private:
	CommentsBoard* m_parent;
    };

private:
    OnVideoSelect* on_select;
    DECLARE_EVENT_TABLE()
};


class CommentsPane : public wxScrolledWindow
{
public:
    CommentsPane(CommentsBoard* parent);

    void AddComment(CommentInfo* comment);
    void RefreshCommentList(); // adds every CommentInfo from the m_comments

    class CommentRect : public wxPanel
    {
    public:
	/*Displays and manages controls for a single video comment.*/
	CommentRect(wxWindow* parent, CommentInfo* comment, wxWindowID id = wxID_ANY);
	~CommentRect();
    protected:
	wxBoxSizer* m_v_sizer;
	wxStaticText* m_comment_txt;
	CommentInfo* m_comment_info;
    };

private:
    CommentsBoard* m_parent;
    wxBoxSizer* m_v_sizer;

};
#endif  // COMMENTS_BOARD_H

