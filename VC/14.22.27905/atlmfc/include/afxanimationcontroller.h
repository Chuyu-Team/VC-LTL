// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include "afxanimationhelper.h"
#include "afxtempl.h"

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

class CBaseKeyFrame;
class CAnimationVariable;

/// <summary>
/// Represents a basic transition.
/// </summary>
/// <remarks>
/// This class encapsulates IUIAnimationTransition interface and serves as a base class for all transitions.
/// </remarks>
class CBaseTransition : public CObject
{
	DECLARE_DYNAMIC(CBaseTransition);
public:
	/// <summary>
	/// Defines the transition types currently supported by the MFC implementation of Windows Animation API.
	/// </summary>
	/// <remarks>
	/// A transition type is set in the constructor of specific transition.
	/// For example, CSinusoidalTransitionFromRange sets its type to SINUSOIDAL_FROM_RANGE.
	/// </remarks>
	enum TRANSITION_TYPE
	{
		UNKNOWN,
		ACCELERATE_DECELERATE,
		CONSTANT,
		CUBIC,
		DISCRETE,
		INSTANTENEOUS,
		LINEAR,
		LINEAR_FROM_SPEED,
		SMOOTH_STOP,
		PARABOLIC_FROM_ACCELERATION,
		REVERSAL,
		SINUSOIDAL_FROM_RANGE,
		SINUSOIDAL_FROM_VELOCITY,
		CUSTOM
	};
public:
	/// <summary>
	/// Constructs a base transtion object.
	/// </summary>
	CBaseTransition() : m_type(CBaseTransition::UNKNOWN), m_pStartKeyframe(NULL), m_pEndKeyframe(NULL),
		m_pRelatedVariable(NULL), m_bAdded(FALSE)
	{
	}

	/// <summary>
	/// The destructor. Called when a transition object is being destroyed.
	/// </summary>
	virtual ~CBaseTransition()
	{
	}

	/// <summary>
	/// Releases encapsulated IUIAnimationTransition COM object.
	/// </summary>
	/// <remarks>
	/// This method should be called from a derived class's Create method in order to prevent
	/// IUITransition interface leak.
	/// </remarks>
	void Clear()
	{
		if (m_transition != NULL)
		{
			m_transition.Detach()->Release();
		}
	}

	/// <summary>
	/// Returns transition type.
	/// </summary>
	/// <returns>
	/// One of TRANSITION_TYPE enumerated values.
	/// </returns>
	/// <remarks>
	/// This method can be used to identify a transition object by its type.
	/// The type is set in a constructor in a derived class.
	/// </remarks>
	TRANSITION_TYPE GetType() const {return m_type;}

	/// <summary>
	/// Creates a COM transition.
	/// </summary>
	/// <returns>
	/// TRUE if a transition COM object was created successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This is a pure virtual function that must be overridden in a derived class.
	/// It's called by the framework to instantiate the underlying COM transition object.
	/// </remarks>
	/// <param name="pLibrary">A pointer to transition library, which creates standard transitions. It can be NULL for custom transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which creates custom transitions. It can be NULL for standard transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory) = 0;

	/// <summary>
	/// Returns a pointer to underlying COM transition object.
	/// </summary>
	/// <returns>
	/// A valid pointer to IUIAnimationTransition or NULL if underlying transition can't be created.
	/// </returns>
	/// <remarks>
	/// This method returns a pointer to underlying COM transition object and creates it if necessary.
	/// </remarks>
	/// <param name="pLibrary">A pointer to transition library, which creates standard transitions. It can be NULL for custom transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which creates custom transitions. It can be NULL for standard transitions.</param>
	IUIAnimationTransition* GetTransition(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory);

	/// <summary>
	/// Returns a pointer to underlying COM transition object.
	/// </summary>
	/// <returns>
	/// A valid pointer to IUIAnimationTransition or NULL if underlying transition can't be created.
	/// </returns>
	/// <remarks>
	/// It's an accessor method to underlying COM transition object. It doesn't instantiates the underlying IUIAnimationTransition COM
	/// object if it wasn't created.
	/// </remarks>
	IUIAnimationTransition* GetTransition()
	{
		return m_transition;
	}

	/// <summary>
	/// Sets keyframes for a transition.
	/// </summary>
	/// <remarks>
	/// This method tells the transition to start after specified keyframe and, optionally, if pEnd is not NULL, end
	/// before the specified keyframe.
	/// If the transition was created with a duration parameter specified, that duration is overwritten with the duration of time between the start and end keyframes.
	/// </remarks>
	/// <param name="pStart">A keyframe that specifies the beginning of the transition. </param>
	/// <param name="pEnd">A keyframe that specifies the end of the transition.</param>
	void SetKeyframes(CBaseKeyFrame* pStart = NULL, CBaseKeyFrame* pEnd = NULL)
	{
		m_pStartKeyframe = pStart;
		m_pEndKeyframe = pEnd;
	}

	/// <summary>
	/// Tells whether a transition has been added to a storyboard.
	/// </summary>
	/// <returns>
	/// Returns TRUE if a transition has been added to a storyboard, otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This flag is set internally when the top level code adds transitions to storyboard.
	/// </remarks>
	BOOL IsAdded(){return m_bAdded;}

	/// <summary>
	/// Returns start keyframe.
	/// </summary>
	/// <returns>
	/// A valid pointer to a keyframe, or NULL if a transition should not start after a keyframe.
	/// </returns>
	/// <remarks>
	/// This method can be used to access a keyframe object that was previously set by SetKeyframes.
	/// It's called by top level code when transitions are being added to storyboard.
	/// </remarks>
	CBaseKeyFrame* GetStartKeyframe() {return m_pStartKeyframe;}

	/// <summary>
	/// Returns start keyframe.
	/// </summary>
	/// <returns>
	/// A valid pointer to a keyframe, or NULL if a transition should not be inserted between keyframes.
	/// </returns>
	/// <remarks>
	/// This method can be used to access a keyframe object that was previously set by SetKeyframes.
	/// It's called by top level code when transitions are being added to storyboard.
	/// </remarks>
	CBaseKeyFrame* GetEndKeyframe() {return m_pEndKeyframe;}

	/// <summary>
	/// Establishes a relationship between animation variable and transition.
	/// </summary>
	/// <remarks>
	/// Establishes a relationship between animation variable and transition. A transition can be applied only to one variable.
	/// </remarks>
	/// <param name="pVariable">A pointer to related animation variable.</param>
	void SetRelatedVariable(CAnimationVariable* pVariable)
	{
		m_pRelatedVariable = pVariable;
	}

	/// <summary>
	/// Returns a pointer to related variable.
	/// </summary>
	/// <returns>
	/// A valid pointer to animation variable, or NULL if an animation variable has not been set by SetRelatedVariable.
	/// </returns>
	/// <remarks>
	/// This is an accessor to related animation variable.
	/// </remarks>
	CAnimationVariable* GetRelatedVariable() {return m_pRelatedVariable;}

	/// <summary>
	/// Adds a transition to a storyboard.
	/// </summary>
	/// <returns>
	/// TRUE, if transition was successfully added to a storyboard.
	/// </returns>
	/// <remarks>
	/// Applies the transition to the related variable in the storyboard.  If this is the first transition
	/// applied to this variable in this storyboard, the transition begins at the start of the storyboard.
	/// Otherwise, the transition is appended to the transition added most recently to the variable.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to storyboard, which will animate the related variable.</param>
	BOOL AddToStoryboard(IUIAnimationStoryboard* pStoryboard);

	/// <summary>
	/// Adds a transition to a storyboard.
	/// </summary>
	/// <returns>
	/// TRUE, if transition was successfully added to a storyboard.
	/// </returns>
	/// <remarks>
	/// Applies the transition to the related variable in the storyboard. If the start keyframe was specified, the transition
	/// begins at that keyframe. If the end keyframe was specified, the transition begins at the start keyframe and
	/// and stops at the end keyframe. If the transition was created with a duration parameter specified, that duration is
	/// overwritten with the duration of time between the start and end keyframes.
	/// If no keyframe was specified, the transition is appended to the transition added most recently to the variable.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to storyboard, which will animate the related variable.</param>
	BOOL AddToStoryboardAtKeyframes(IUIAnimationStoryboard* pStoryboard);

protected:
	/// <summary>
	/// Stores the transition type.
	/// </summary>
	TRANSITION_TYPE 				m_type;

	/// <summary>
	/// Stores a pointer to IUIAnimationTransition. NULL if a COM transition object has not been created.
	/// </summary>
	ATL::CComPtr<IUIAnimationTransition> m_transition;

	/// <summary>
	/// Stores a pointer to the keyframe that specifies the beginning of the transition.
	/// </summary>
	CBaseKeyFrame* 					m_pStartKeyframe;

	/// <summary>
	/// Stores a pointer to the keyframe that specifies the end of the transition.
	/// </summary>
	CBaseKeyFrame* 					m_pEndKeyframe;

	/// <summary>
	/// A pointer to an animation variable, which is animated with the transition stored in m_transition.
	/// </summary>
	CAnimationVariable*		 		m_pRelatedVariable;

	/// <summary>
	/// Specifies whether a transition has been added to a storyboard.
	/// </summary>
	BOOL							m_bAdded;
};

/// <summary>
/// Implements basic functionality of keyframe.
/// </summary>
/// <remarks>
/// Encapsulates UI_ANIMATION_KEYFRAME variable. Serves as a base class for any keyframe implementation.
/// A keyframe represents a moment in time within a storyboard and can be used to specify the start and end times of transitions.
/// There are two types of keyframes - keyframes added to storyboard at the specified offset (in time), or keyframes added
/// after specified transition. Because durations of some transitions can't be known before animation starts, the actual values
/// of some keyframes are determined at runtime only.
/// Because keyframes may depend on transitions, which in their turn depend on keyframes, it's important to prevent
/// infinite recursions when building keyframe chains.
/// </remarks>
class CBaseKeyFrame : public CObject
{
	DECLARE_DYNAMIC(CBaseKeyFrame);
	friend class CAnimationGroup;
protected:
	/// <summary>
	/// Represents a Windows Animation API keyframe. When a keyframe is not initialized
	/// it is set to the predefined value UI_ANIMATION_KEYFRAME_STORYBOARD_START.
	/// </summary>
	UI_ANIMATION_KEYFRAME	m_keyframe;

	/// <summary>
	/// Specifies whether this keyframe has been added to a storyboard.
	/// </summary>
	BOOL					m_bAdded;

	/// <summary>
	/// Specifies whether this keyframe should be added to storyboard at an offset from
	/// another existing keyframe, or at the end of some transition.
	/// </summary>
	BOOL					m_bIsKeyframeAtOffset;
public:
	/// <summary>
	/// Constructs a keyframe object.
	/// </summary>
	CBaseKeyFrame() : m_keyframe(UI_ANIMATION_KEYFRAME_STORYBOARD_START), m_bAdded(TRUE)
	{
		m_keyframe = UI_ANIMATION_KEYFRAME_STORYBOARD_START;
		m_bIsKeyframeAtOffset = TRUE;
	}

	/// <summary>
	/// Returns the underlying keyframe value.
	/// </summary>
	/// <returns>
	/// A current keyframe. The default value is UI_ANIMATION_KEYFRAME_STORYBOARD_START.
	/// </returns>
	/// <remarks>
	/// This is an accessor to the underlying keyframe value.
	/// </remarks>
	UI_ANIMATION_KEYFRAME GetAnimationKeyframe() const {return m_keyframe;}

	/// <summary>
	/// Tells whether a keyframe has been added to storyboard.
	/// </summary>
	/// <returns>
	/// TRUE if a keyframe is added to a storyboard; otehrwise FALSE.
	/// </returns>
	/// <remarks>
	/// In the base class IsAdded always returns TRUE, but it's overridden in derived classes.
	/// </remarks>
	BOOL IsAdded() const {return m_bAdded;}

	/// <summary>
	/// Specifies whether the keyframe should be added to storyboard at offset, or after transition.
	/// </summary>
	/// <returns>
	/// TRUE if the keyframe should be added to storyboard at some specified offset.
	/// FALSE if the keyframe should be added to storyboard after some transition.
	/// </returns>
	/// <remarks>
	/// Specifies whether the keyframe should be added to storyboard at offset.
	/// The offset or transition must be specified in a derived class.
	/// </remarks>
	BOOL IsKeyframeAtOffset() const {return m_bIsKeyframeAtOffset;}

	/// <summary>
	/// Adds a keyframe to storyboard.
	/// </summary>
	/// <returns>
	/// TRUE if keyframe was added to storyboard successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method is called to add a keyframe to storyboard.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	/// <param name="bDeepAdd">If this parameter is TRUE and the keyframe being added depends on some other keyframe or transition, this method tries
	/// to add this keyframe or transition to storyboard first.</param>
	virtual BOOL AddToStoryboard(IUIAnimationStoryboard* pStoryboard, BOOL bDeepAdd);
};

/// <summary>
/// Represents an animation keyframe.
/// </summary>
/// <remarks>
/// This class implements an animation keyframe. A keyframe represents a moment in time within a
/// storyboard and can be used to specify the start and end times of transitions.
/// A keyframe may be based on other keyframe and have an offset (in seconds) from it, or may be based on a transition and
/// represent a moment in time when this transition ends.
/// </remarks>
class CKeyFrame : public CBaseKeyFrame
{
	DECLARE_DYNAMIC(CKeyFrame);
protected:

	/// <summary>
	/// Stores a pointer to transtion that begins at this keyframe.
	/// </summary>
	CBaseTransition*		m_pTransition;

	/// <summary>
	/// Specifies offset of this keyframe from a keyframe stored in m_pExistingKeyFrame.
	/// </summary>
	UI_ANIMATION_SECONDS 	m_offset;

	/// <summary>
	/// Stores a pointer to an existing keframe. This keyframe is added to storyboard with m_offset to the existing keyframe.
	/// </summary>
	CBaseKeyFrame* 			m_pExistingKeyFrame;

public:
	/// <summary>
	/// Constructs a keyframe that depends on a transition.
	/// </summary>
	/// <remarks>
	/// The constructed keyframe will represent a moment in time within a storyboard when the specified transition ends.
	/// </remarks>
	/// <param name="pTransition">A pointer to a transition.</param>
	CKeyFrame(CBaseTransition* pTransition);

	/// <summary>
	/// Constructs a keyframe that depends on other keyframe.
	/// </summary>
	/// <remarks>
	/// The constructed keyframe will represent a moment in time within a storyboard, which has a specified offset from pKeyframe.
	/// </remarks>
	/// <param name="pKeyframe">A pointer to keyframe.</param>
	/// <param name="offset">Offset, in seconds, from keyframe specified by pKeyframe.</param>
	CKeyFrame(CBaseKeyFrame* pKeyframe, UI_ANIMATION_SECONDS offset = 0.0);

	/// <summary>
	/// Returns a pointer to a transition this keyframe depends on.
	/// </summary>
	/// <returns>
	/// A valid pointer to transition, or NULL if this keyframe does not depend on transition.
	/// </returns>
	/// <remarks>
	/// This is an accessor to a transition this keyframe depends on.
	/// </remarks>
	CBaseTransition* GetTransition() {return m_pTransition;}

	/// <summary>
	/// Returns a pointer to a keyframe this keyframe depends on.
	/// </summary>
	/// <returns>
	/// A valid pointer to keyframe, or NULL if this keyframe does not depend on other keyframe.
	/// </returns>
	/// <remarks>
	/// This is an accessor to a keyframe this keyframe depends on.
	/// </remarks>
	CBaseKeyFrame* GetExistingKeyframe() {return m_pExistingKeyFrame;}

	/// <summary>
	/// Returns an offset from other keyframe.
	/// </summary>
	/// <returns>
	/// An offset in seconds from other keyframe.
	/// </returns>
	/// <remarks>
	/// This method should be called to determine an offset in seconds from other keyframe.
	/// </remarks>
	UI_ANIMATION_SECONDS GetOffset() {return m_offset;}

	/// <summary>
	/// Adds a keyframe to a storyboard.
	/// </summary>
	/// <returns>
	/// TRUE, if keyframe was added successfully.
	/// </returns>
	/// <remarks>
	/// This method adds a keyframe to storyboard. If it depends on other keyframe or transition and bDeepAdd is TRUE,
	/// this method tries to add them recursively.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	/// <param name="bDeepAdd">Specifies whether to add keyframe or transition recursively.</param>
	virtual BOOL AddToStoryboard(IUIAnimationStoryboard* pStoryboard, BOOL bDeepAdd);

	/// <summary>
	/// Adds a keyframe to storyboard after transition.
	/// </summary>
	/// <returns>
	/// TRUE, if keyframe was added successfully.
	/// </returns>
	/// <remarks>
	/// This function is called by the framework to add a keyframe to storyboard after transition.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	/// <param name="bDeepAdd">Specifies whether to add a transition recursively.</param>
	BOOL AddToStoryboardAfterTransition(IUIAnimationStoryboard* pStoryboard, BOOL bDeepAdd);

	/// <summary>
	/// Adds a keyframe to storyboard at offset.
	/// </summary>
	/// <returns>
	/// TRUE, if keyframe was added successfully.
	/// </returns>
	/// <remarks>
	/// This function is called by the framework to add a keyframe to storyboard at offset.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	/// <param name="bDeepAdd">Specifies whether to add a keyframe this keyframe depend on recursively.</param>
	virtual BOOL AddToStoryboardAtOffset(IUIAnimationStoryboard* pStoryboard, BOOL bDeepAdd);
 };

class CAnimationController;
class CAnimationBaseObject;

/// <summary>
/// Represents an animation variable.
/// </summary>
/// <remarks>
/// The CAnimationVariable class encapsulates IUIAnimationVariable COM object. It also holds a list of transitions
/// to be applied to the animation variable in a storyboard. CAnimationVariable objects are embedded to
/// animation objects, which can represent in an application an animated value, point, size, color and rectangle.
/// </remarks>
class CAnimationVariable
{
	friend class CAnimationBaseObject;
protected:
	/// <summary>
	/// Stores a pointer to IUIAnimationVariable COM object. NULL if the COM object has not been created yet, or if creation failed.
	/// </summary>
	ATL::CComPtr<IUIAnimationVariable> 	m_variable;

	/// <summary>
	/// Specifies the default value, which is propagated to IUIAnimationVariable.
	/// </summary>
	DOUBLE 							m_dblDefaultValue;

	/// <summary>
	/// Contains a list of transitions that animate this animation variable.
	/// </summary>
	CObList 						m_lstTransitions;

	/// <summary>
	/// A pointer to an animation object that encapsulates this animation variable.
	/// </summary>
	CAnimationBaseObject*			m_pParentObject;

protected:
	/// <summary>
	/// Sets the relationship between an animation variable and an animation object.
	/// </summary>
	/// <remarks>
	/// This method is called internally to establish one-to-one relationship between an animation variable and an animation object that encapsulates it.
	/// </remarks>
	/// <param name="pParentObject">A pointer to an animation object that contains this variable.</param>
	void SetParentAnimationObject(CAnimationBaseObject* pParentObject) {m_pParentObject = pParentObject;}

public:
	/// <summary>
	/// Constructs an animation variable object.
	/// </summary>
	/// <remarks>
	/// Constructs an animation variable object and sets its default value. A default value is used when
	/// a variable is not animated, or can't be animated.
	/// </remarks>
	/// <param name="dblDefaultValue">Specifies the default value.</param>
	CAnimationVariable(DOUBLE dblDefaultValue = 0.0);

	/// <summary>
	/// The destructor. Called when a CAnimationVariable object is being destroyed.
	/// </summary>
	virtual ~CAnimationVariable();

	/// <summary>
	/// Returns the parent animation object.
	/// </summary>
	/// <returns>
	/// A pointer to parent animation object, if relationship was established, otherwise NULL.
	/// </returns>
	/// <remarks>
	/// This method can be called to retrieve a pointer to a parent animation object (a container).
	/// </remarks>
	CAnimationBaseObject* GetParentAnimationObject() {return m_pParentObject;}

	/// <summary>
	/// Returns a pointer to IUIAnimationVariable COM object.
	/// </summary>
	/// <returns>
	/// A valid pointer to IUIAnimationVariable COM object, or NULL if animation variable was not created, or can't be created.
	/// </returns>
	/// <remarks>
	/// Use this function to access the underlying IUIAnimationVariable COM object and call its methods directly if needed.
	/// </remarks>
	IUIAnimationVariable* GetVariable() {return m_variable;}

	/// <summary>
	/// Sets default value and releases IUIAnimationVariable COM object.
	/// </summary>
	/// <remarks>
	/// Use this method to reset the default value. This method releases the internal IUIAnimationVariable COM object,
	/// therefore when animation variable is recreated, the underlying COM object gets the new default value.
	/// The default value is returned by GetValue if the COM object representing the animation variable is not created,
	/// or if the variable has not been animated.
	/// </remarks>
	/// <param name="dblDefaultValue">Specifies the new default value.</param>
	void SetDefaultValue(DOUBLE dblDefaultValue);

	/// <summary>
	/// Creates the underlying animation variable COM object.
	/// </summary>
	/// <returns>
	/// TRUE if the animation variable was successfully created; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method creates the underlying animation variable COM object and sets its default value.
	/// </remarks>
	/// <param name="pManager">A pointer to animation manager.</param>
	virtual BOOL Create(IUIAnimationManager* pManager);

	/// <summary>
	/// Creates all transitions to be applied to this animation variable.
	/// </summary>
	/// <returns>
	/// TRUE if transitions were created successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method is called by the framework when it needs to create transitions that have been added
	/// to the variable's internal list of transitions.
	/// </remarks>
	/// <param name="pLibrary">A pointer to transition library.</param>
	/// <param name="pFactory">A pointer to transition factory.</param>
	BOOL CreateTransitions(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory);

	/// <summary>
	/// Returns the current value of animation variable.
	/// </summary>
	/// <returns>
	/// S_OK if the value was obtained successfully, or underlying animation variable has not been created. Otherwise
	/// HRESULT error code.
	/// </returns>
	/// <remarks>
	/// This method can be called to retrieve the current value of animation variable. If the underlying COM object
	/// has not been created, dblValue will contain a default value, when the function returns.
	/// </remarks>
	/// <param name="dblValue">The current value of the animation variable.</param>
	HRESULT GetValue(DOUBLE& dblValue);

	/// <summary>
	/// Returns the current value of animation variable.
	/// </summary>
	/// <returns>
	/// S_OK if the value was obtained successfully, or underlying animation variable has not been created. Otherwise
	/// HRESULT error code.
	/// </returns>
	/// <remarks>
	/// This method can be called to retrieve the current value of animation variable. If the underlying COM object
	/// has not been created, dblValue will contain a default value, when the function returns.
	/// </remarks>
	/// <param name="nValue">The current value of the animation variable.</param>
	HRESULT GetValue(INT32& nValue);

	/// <summary>
	/// Adds a transition.
	/// </summary>
	/// <remarks>
	/// This method is called to add a transition to the internal list of transitions to be applied to the animation variable.
	/// This list should be cleared when an animation has been scheduled.
	/// </remarks>
	/// <param name="pTransition">A pointer to a transition to add.</param>
	void AddTransition(CBaseTransition* pTransition);

	/// <summary>
	/// Clears transitions.
	/// </summary>
	/// <remarks>
	/// This method removes all transitions from the internal list of transitions. If bAutodestroy is TRUE, or
	/// m_bAutodestroyTransitions is TRUE, then transitions are deleted. Otherwise the caller should deallocate
	/// the transition objects.
	/// </remarks>
	/// <param name="bAutodestroy">Specifies whether this method should delete transition objects.</param>
	void ClearTransitions(BOOL bAutodestroy);

	/// <summary>
	/// Adds transitions from the internal list to storyboard.
	/// </summary>
	/// <remarks>
	/// This method adds transitions from the internal list to storyboard. It's called from the
	/// top level code several times to add transitions that do not depend on keyframes and add
	/// transitions that depend on keyframes. If the underlying animation variable
	/// COM object has not been created, this method creates it at this stage.
	/// </remarks>
	/// <param name="pController">A pointer to parent animation controller.</param>
	/// <param name="pStoryboard">A pointer to storyboard.</param>
	/// <param name="bDependOnKeyframes">TRUE, if this method should add transitions that depend on keyframes.</param>
	void ApplyTransitions(CAnimationController* pController, IUIAnimationStoryboard* pStoryboard, BOOL bDependOnKeyframes);

	/// <summary>
	/// Enables or disables the ValueChanged event.
	/// </summary>
	/// <remarks>
	/// When ValueChanged event is enabled, the framework calls virtual method CAnimationController::OnAnimationValueChanged.
	/// You need to override it in a class derived from CAnimationController in order to process this event. This method is called every time
	/// the value of animation variable is changed.
	/// </remarks>
	/// <param name="pController">A pointer to parent controller.</param>
	/// <param name="bEnable">TRUE - enable event, FALSE - disable event.</param>
	void EnableValueChangedEvent (CAnimationController* pController, BOOL bEnable);

	/// <summary>
	/// Enables or disables the IntegerValueChanged event.
	/// </summary>
	/// <remarks>
	/// When ValueChanged event is enabled, the framework calls virtual method CAnimationController::OnAnimationIntegerValueChanged.
	/// You need to override it in a class derived from CAnimationController in order to process this event. This method is called every time
	/// the integer value of animation variable is changed.
	/// </remarks>
	/// <param name="pController">A pointer to parent controller.</param>
	/// <param name="bEnable">TRUE - enable event, FALSE - disable event.</param>
	void EnableIntegerValueChangedEvent (CAnimationController* pController, BOOL bEnable);

	/// <summary>
	/// Returns default value.
	/// </summary>
	/// <returns>
	/// The default value.
	/// </returns>
	/// <remarks>
	/// Use this function to obtain default value of animation variable. The default value can be set in constructor or by
	/// SetDefaultValue method.
	/// </remarks>
	DOUBLE GetDefaultValue() const {return m_dblDefaultValue;}

public:
	/// <summary>
	/// Specifies whether related transition objects should be deleted.
	/// </summary>
	/// <remarks>
	/// Set this value to TRUE to force deletion of transition objects when they are being removed from the internal list
	/// of transitions. If this value is FALSE the transitions should be deleted by calling application.
	/// The list of transitions is always cleared after an animation has been scheduled. The default value is FALSE.
	/// </remarks>
	BOOL	m_bAutodestroyTransitions;
};

/// <summary>
/// The base class for all animation objects.
/// </summary>
/// <remarks>
/// This class implements basic methods for all animation objects. An animation object can represent a value, point,
/// size, rectangle or color in an application, as well as any custom entity. Animation objects are stored in animation groups
/// (see CAnimationGroup). Each group can be animated separately and can be treated as an analogue of storyboard.
/// An animation object encapsulates one or more animation variables (see CAnimationVariable), depending on its logical
/// representation. For example, CAnimationRect contains four animation variables - one variable for each side of rectangle.
/// Each animation object class exposes overloaded AddTransition method, which should be used to apply transitions to
/// encapsulated animation variables.
/// An animation object can be identified by Object ID (optionally) and by Group ID. A Group ID is necessary in order
/// to place an animation object to correct group, but if a Group ID is not specified, an object is placed in the default group with ID 0.
/// If you call SetID with different GroupID, an animation object will be moved to another group (a new group is created if necessary).
/// </remarks>
class CAnimationBaseObject : public CObject
{
	DECLARE_DYNAMIC(CAnimationBaseObject)
	friend class CAnimationController;

protected:
	/// <summary>
	/// Specifies the Group ID of the animation object.
	/// </summary>
	UINT32 	m_nGroupID;

	/// <summary>
	/// Specifes the Object ID of the animation object.
	/// </summary>
	UINT32 	m_nObjectID;

	/// <summary>
	/// Stores user-defined data.
	/// </summary>
	DWORD_PTR 	m_dwUserData;

	/// <summary>
	/// Specifies whether related transitions should be automatically destroyed.
	/// </summary>
	BOOL 	m_bAutodestroyTransitions;

	/// <summary>
	/// A pointer to the parent animation controller.
	/// </summary>
	CAnimationController* m_pParentController;

public:
	/// <summary>
	/// Constructs an animation object.
	/// </summary>
	/// <remarks>
	/// Constructs an animation objects and assigns default Object ID (0) and Group ID (0).
	/// </remarks>
	CAnimationBaseObject();

	/// <summary>
	/// Constructs an animation object.
	/// </summary>
	/// <remarks>
	/// Constructs an animation object with specified Object ID and Group ID.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">User-defined data, which can be associated with animation object and retrieved later at runtime.</param>
	CAnimationBaseObject(UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// The destructor. Called when an animation object is being destroyed.
	/// </summary>
	virtual ~CAnimationBaseObject();

	/// <summary>
	/// Creates transitions associated with an animation object.
	/// </summary>
	/// <returns>
	/// TRUE if transitions were created successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Loops over list of animation variables encapsulated in a derived animation object and creates transitions
	/// associated with each animation variable.
	/// </remarks>
	BOOL CreateTransitions();

	/// <summary>
	/// Sets new IDs.
	/// </summary>
	/// <remarks>
	/// Allows to change Object ID and Group ID. If the new Group ID differs from the current ID, an animation object
	/// is moved to another group (a new group will be created, if necessary).
	/// </remarks>
	/// <param name="nObjectID">Specifies new Object ID.</param>
	/// <param name="nGroupID">Specifies new Group ID.</param>
	void SetID(UINT32 nObjectID, UINT32 nGroupID = 0);

	/// <summary>
	/// Sets user-defined data.
	/// </summary>
	/// <remarks>
	/// Use this method to associate a custom data with an animation object. This data may be retrieved later at runtime by GetUserData.
	/// </remarks>
	/// <param name="dwUserData">Specifies the custom data.</param>
	void SetUserData (DWORD_PTR dwUserData)
	{
		m_dwUserData = dwUserData;
	}

	/// <summary>
	/// Detaches an animation object from parent animation controller.
	/// </summary>
	/// <remarks>
	/// This method is used internally.
	/// </remarks>
	void DetachFromController()
	{
		m_pParentController = NULL;
	}

	/// <summary>
	/// Returns current Object ID.
	/// </summary>
	/// <returns>
	/// Current Object ID.
	/// </returns>
	/// <remarks>
	/// Use this method to retrieve Object ID. It's 0 if Object ID has not been set explicitly in constructor or with SetID.
	/// </remarks>
	UINT32 GetObjectID() const {return m_nObjectID;}

	/// <summary>
	/// Returns current Group ID.
	/// </summary>
	/// <returns>
	/// Current Group ID.
	/// </returns>
	/// <remarks>
	/// Use this method to retrieve Group ID. It's 0 if Group ID has not been set explicitly in constructor or with SetID.
	/// </remarks>
	UINT32 GetGroupID() const {return m_nGroupID;}

	/// <summary>
	/// Returns user defined data.
	/// </summary>
	/// <returns>
	/// A value of custom data.
	/// </returns>
	/// <remarks>
	/// Call this method to retrieve the custom data at runtime. The returned value will be 0 if it was not explicitly initialized in constructor or with SetUserData.
	/// </remarks>
	DWORD_PTR GetUserData() const {return m_dwUserData;}

	/// <summary>
	/// Sets a flag that orders to automatically destroy transitions.
	/// </summary>
	/// <remarks>
	/// Set this flag only if you allocated transition objects using operator new. If for some reason transition
	/// objects are allocated on the stack, the auto destroy flag should be FALSE. By default this flag is TRUE.
	/// </remarks>
	/// <param name="bValue">Specifies the auto destroy flag.</param>
	void SetAutodestroyTransitions(BOOL bValue)
	{
		m_bAutodestroyTransitions = bValue;
	}

	/// <summary>
	/// Tells whether related transition are destroyed automatically.
	/// </summary>
	/// <returns>
	/// If TRUE, related transitions are destroyed automatically; if FALSE, transition objects should be deallocated by calling application.
	/// </returns>
	/// <remarks>
	/// By default this flag is TRUE. Set this flag only if you allocated transition on the stack and/or transitions should be deallocated by
	/// the calling application.
	/// </remarks>
	BOOL GetAutodestroyTransitions() const
	{
		return m_bAutodestroyTransitions;
	}

	/// <summary>
	/// Adds transitions to storyboard with encapsulated animation variable.
	/// </summary>
	/// <returns>
	/// TRUE if transitions were added successfully.
	/// </returns>
	/// <remarks>
	/// Adds related transitions, that have been added with AddTransition (overloaded methods in derived classes), to storyboard.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	/// <param name="bDependOnKeyframes">With FALSE this method adds only those transitions that do not depend on keyframes.</param>
	virtual BOOL ApplyTransitions(IUIAnimationStoryboard* pStoryboard, BOOL bDependOnKeyframes);

	/// <summary>
	/// Removes all related transitions.
	/// </summary>
	/// <remarks>
	/// Removes all related transitions and destroys them if bAutodestroy or m_bAutodestroyTransitions flag is TRUE.
	/// Transitions should be destroyed automatically only if they are not allocated on the stack.
	/// If the above flags are FALSE, transitions are just removed from the internal list of related transitions.
	/// </remarks>
	/// <param name="bAutodestroy">Specifies whether to destroy transition objects automatically, or just remove them from the related list.</param>
	virtual void ClearTransitions(BOOL bAutodestroy);

	/// <summary>
	/// Sets up Value Changed event handler.
	/// </summary>
	/// <remarks>
	/// If the Value Changed event handler is enabled, you can handle this event in CAnimationController::OnAnimationValueChanged method,
	/// which should be overridden in a CAnimationController-derived class. This method is called every time the animation value has changed.
	/// </remarks>
	/// <param name="pController">A pointer to a parent controller.</param>
	/// <param name="bEnable">Specifies whether to enable, or disable Value Changed event.</param>
	virtual void EnableValueChangedEvent(CAnimationController* pController, BOOL bEnable);

	/// <summary>
	/// Sets up Integer Value Changed event handler.
	/// </summary>
	/// <remarks>
	/// If the Integer Value Changed event handler is enabled, you can handle this event in CAnimationController::OnAnimationIntegerValueChanged method,
	/// which should be overridden in a CAnimationController-derived class. This method is called every time the animation integer value has changed.
	/// </remarks>
	/// <param name="pController">A pointer to a parent controller.</param>
	/// <param name="bEnable">Specifies whether to enable, or disable Integer Value Changed event.</param>
	virtual void EnableIntegerValueChangedEvent(CAnimationController* pController, BOOL bEnable);

	/// <summary>
	/// Determines whether an animation object contains a particular animation variable.
	/// </summary>
	/// <returns>
	/// TRUE if the animation variable is contained in the animation object; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method can be used to determine whether an animation variable specified by pVariable is contained
	/// within an animation object. An animation object, depending on its type, may contain several animation variables.
	/// For example, CAnimationColor contains three variables, one for each color component (red, green and blue). When a
	/// value of animation variable has changed, Windows Animation API sends ValueChanged or IntegerValueChanged events (if enabled),
	/// and the parameter of this event is a pointer to interface IUIAnimationVariable of animation variable.
	/// This method helps to obtain a pointer to animation from a pointer to contained COM object.
	/// </remarks>
	/// <param name="pVariable">A pointer to animation variable.</param>
	virtual BOOL ContainsVariable(IUIAnimationVariable* pVariable);

protected:
	/// <summary>
	/// Collects pointers to contained animation variables.
	/// </summary>
	/// <remarks>
	/// This is a pure virtual method that must be overridden in a derived class.
	/// An animation object, depending on its type, contains one or more animation variables. For example, CAnimationPoint
	/// contains two variables, for X and Y coordinates respectively. The base class CAnimationBaseObject implements
	/// some generic methods, which act on a list of animation variables: ApplyTransitions, ClearTransitions,
	/// EnableValueChangedEvent, EnableIntegerValueChangedEvent. These methods call GetAnimationVariableList, which is filled
	/// in a derived class with actual animation variables contained in a particular animation object, then loop over the
	/// list and perform necessary actions.
	/// If you create a custom animation object, you must add to lst all animation variables contained in that object.
	/// </remarks>
	/// <param name="lst">A list that must be filled with animation variables contained in an animation object.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst) = 0;

	/// <summary>
	/// Establishes relationship between animation variables, contained in an animation object, and their container.
	/// </summary>
	/// <remarks>
	/// This is a helper that can be used to establish relationship between animation variables, contained in an animation object,
	/// and their container. It loops over animation variables and sets a back pointer to a parent animation object
	/// to each animation variable.
	/// In the current implementation the actual relationship is established in CAnimationBaseObject::ApplyTransitions, therefore
	/// back pointers are not set until you call CAnimationGroup::Animate.
	/// Knowing the relationship may be helpful when you processing events and need to get a parent animation object
	/// from CAnimationVariable (use CAnimationVariable::GetParentAnimationObject).
	/// </remarks>
	virtual void SetParentAnimationObjects();
};

/// <summary>
/// Implements the functionality of animation object with one value.
/// </summary>
/// <remarks>
/// The CAnimationValue class encapsulates a single CAnimationVariable object and can represent in applications
/// a single animated value. For example, you can use this class for animated transparency (fade effect), angle (to rotate objects),
/// or for any other case when you need to create an animation depending on a single animated value.
/// To use this class in application, just instantiate an object of this class, add it to animation controller using
/// CAnimationController::AddAnimationObject and call AddTransition for each transition to be applied to the value.
/// </remarks>
class CAnimationValue : public CAnimationBaseObject
{
	DECLARE_DYNAMIC(CAnimationValue)
protected:
	/// <summary>
	/// The encapsulated animation variable that represents animation value.
	/// </summary>
	CAnimationVariable 	m_value;

public:
	/// <summary>
	/// Constructs a CAnimationValue object.
	/// </summary>
	/// <remarks>
	/// Constructs CAnimationValue object with default properties: default value, Group ID and Object ID are set to 0.
	/// </remarks>
	CAnimationValue();

	/// <summary>
	/// Constructs a CAnimationValue object.
	/// </summary>
	/// <remarks>
	/// Constructs CAnimationValue object with specified properties.
	/// </remarks>
	/// <param name="dblDefaultValue">Specifies default value.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">specifies user-defined data.</param>
	CAnimationValue(DOUBLE dblDefaultValue, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Sets default value.
	/// </summary>
	/// <remarks>
	/// Use this method to set a default value. A default value is returned to application when animation has not been started
	/// and/or underlying COM object has not been created. If the underlying COM object encapsulated in CAnimationVarible
	/// was already created, this method recreates it, therefore you might need to call EnableValueChanged/EnableIntegerValueChanged
	/// methods again.
	/// </remarks>
	/// <param name="dblDefaultValue">Specifies the default value.</param>
	void SetDefaultValue(DOUBLE dblDefaultValue)
	{
		m_value.SetDefaultValue(dblDefaultValue);
	}

	/// <summary>
	/// Retrieves the current value.
	/// </summary>
	/// <returns>
	/// TRUE if the current value was retrieved successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value. This implementation calls the encapsulated COM object, and if
	/// the call fails, this method returns the default value that was previously set in constructor or with SetDefaultValue.
	/// </remarks>
	/// <param name="dblValue">Output. When the function returns it contains a current value of animation variable.</param>
	BOOL GetValue(DOUBLE& dblValue);

	/// <summary>
	/// Retrieves the current value.
	/// </summary>
	/// <returns>
	/// TRUE if the current value was retrieved successfully; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value. This implementation calls the encapsulated COM object, and if
	/// the call fails, this method returns the default value that was previously set in constructor or with SetDefaultValue.
	/// </remarks>
	/// <param name="nValue">Output. When the function returns it contains a current value of animation variable.</param>
	BOOL GetValue(INT32& nValue);

	/// <summary>
	/// Provides conversion between CAnimationValue and DOUBLE.
	/// </summary>
	/// <returns>
	/// Current value of Animation Value.
	/// </returns>
	/// <remarks>
	/// Provides conversion between CAnimationValue and DOUBLE. This method internally calls GetValue and doesn't check for
	/// errors. If GetValue fails, the returned value will contain a default value previously set in constructor or with SetDefaultValue.
	/// </remarks>
	operator DOUBLE()
	{
		DOUBLE dblVal = 0.;
		GetValue(dblVal);
		return dblVal;
	}

	/// <summary>
	/// Provides conversion between CAnimationValue and INT32.
	/// </summary>
	/// <returns>
	/// Current value of Animation Value as integer.
	/// </returns>
	/// <remarks>
	/// Provides conversion between CAnimationValue and INT32. This method internally calls GetValue and doesn't check for
	/// errors. If GetValue fails, the returned value will contain a default value previously set in constructor or with SetDefaultValue.
	/// </remarks>
	operator INT32()
	{
		INT32 nVal = 0;
		GetValue(nVal);
		return nVal;
	}

	/// <summary>
	/// Assigns a DOUBLE value to CAnimationValue.
	/// </summary>
	/// <remarks>
	/// Assigns a DOUBLE value to CAnimationValue. This value is set as a default value for encapsulated animation variable.
	/// If you subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="dblVal">Specifies the value to be assigned to Animation Value.</param>
	void operator=(DOUBLE dblVal)
	{
		SetDefaultValue(dblVal);
	}

	/// <summary>
	/// Assigns an INT32 value to CAnimationValue.
	/// </summary>
	/// <remarks>
	/// Assigns an INT32 value to CAnimationValue. This value is set as a default value for encapsulated animation variable.
	/// If you subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="nVal">Specifies the value to be assigned to Animation Value.</param>
	void operator=(INT32 nVal)
	{
		SetDefaultValue((DOUBLE)nVal);
	}

	/// <summary>
	/// Provides access to encapsulated animation variable.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated animation variable.
	/// </returns>
	/// <remarks>
	/// Use this method to access the encapsulated animation variable. From CAnimationVariable you get access
	/// to underlying IUIAnimationVariable object, whose pointer can be NULL if animation variable has not been created.
	/// </remarks>
	CAnimationVariable& GetVariable() {return m_value;}

	/// <summary>
	/// Adds a transition to be applied to a value.
	/// </summary>
	/// <remarks>
	/// Call this function to add a transition to internal list of transitions to be applied to an animation variable.
	/// When you add transitions, they are not applied immediately and stored in an internal list. Transitions are applied
	/// (added to a storyboard for a particular value) when you call CAnimationController::AnimateGroup.
	/// </remarks>
	/// <param name="pTransition">A pointer to transition object.</param>
	void AddTransition(CBaseTransition* pTransition);

protected:
	/// <summary>
	/// Puts the encapsulated animation variable into a list.
	/// </summary>
	/// <param name="lst">When the function returns, it contains a pointer to CAnimationVariable representing the animated value.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst);
};

/// <summary>
/// Implements the functionality of a point whose coordinates can be animated.
/// </summary>
/// <remarks>
/// The CAnimationPoint class encapsulates two CAnimationVariable objects and can represent in applications
/// a point. For example, you can use this class to animate a position of any object on the screen (like text string, circle, point etc).
/// To use this class in application, just instantiate an object of this class, add it to animation controller using
/// CAnimationController::AddAnimationObject and call AddTransition for each transition to be applied to X and/or Y coordinates.
/// </remarks>
class CAnimationPoint : public CAnimationBaseObject
{
	DECLARE_DYNAMIC(CAnimationPoint)
protected:
	/// <summary>
	/// The encapsulated animation variable that represents X coordinate of animation point.
	/// </summary>
	CAnimationVariable 	m_xValue;

	/// <summary>
	/// The encapsulated animation variable that represents Y coordinate of animation point.
	/// </summary>
	CAnimationVariable 	m_yValue;

public:
	/// <summary>
	/// Constructs CAnimationPoint object.
	/// </summary>
	/// <remarks>
	/// Constructs CAnimationPoint object with default properties: default point coordinates, Group ID and Object ID are set to 0.
	/// </remarks>
	CAnimationPoint();

	/// <summary>
	/// Constructs CAnimationPoint object.
	/// </summary>
	/// <remarks>
	/// Constructs CAnimationPoint object with specified properties.
	/// </remarks>
	/// <param name="ptDefault">Specifies default point coordinates.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationPoint(const CPoint& ptDefault, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Sets default value.
	/// </summary>
	/// <remarks>
	/// Use this function to set a default value to animation object. This methods assigns default values to X and Y
	/// coordinates of animation point. It also recreates underlying COM objects if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="ptDefault">Specifies the default point value.</param>
	void SetDefaultValue(const POINT& ptDefault);

	/// <summary>
	/// Returns the default values for X and Y coordinates.
	/// </summary>
	/// <returns>
	/// A point containing default value.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve default value, which was previously set by constructor or SetDefaultValue.
	/// </remarks>
	CPoint GetDefaultValue();

	/// <summary>
	/// Returns current value.
	/// </summary>
	/// <returns>
	/// TRUE, if the current value was successfully retrieved; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value of animation point. If this method fails or underlying COM objects for
	/// X and Y coordinates have not been initialized, ptValue contains default
	/// value, which was previously set in constructor or by SetDefaultValue.
	/// </remarks>
	/// <param name="ptValue">Output. Contains the current value when this method returns.</param>
	BOOL GetValue(CPoint& ptValue);

	/// <summary>
	/// Assigns ptSrc to CAnimationPoint.
	/// </summary>
	/// <remarks>
	/// Assigns ptSrc to CAnimationPoint. It's recommended to do that before animation start, because this operator calls
	/// SetDefaultValue, which recreates the underlying COM objects for X and Y coordinates if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="ptSrc">Refers to CPoint or POINT.</param>
	void operator=(const CPoint& ptSrc)
	{
		SetDefaultValue(ptSrc);
	}

	/// <summary>
	/// Converts a CAnimationPoint to a CPoint.
	/// </summary>
	/// <returns>
	/// Current value of CAnimationPoint as CPoint.
	/// </returns>
	/// <remarks>
	/// This function internally calls GetValue. If GetValue for some reason fails, the returned point will contain default values for X and Y coordinates.
	/// </remarks>
	operator CPoint()
	{
		CPoint pt;
		GetValue(pt);
		return pt;
	}

	/// <summary>
	/// Provides access to CAnimationVariable for X coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing X coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing X coordinate.
	/// </remarks>
	CAnimationVariable& GetX() {return m_xValue;}

	/// <summary>
	/// Provides access to CAnimationVariable for Y coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Y coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Y coordinate.
	/// </remarks>
	CAnimationVariable& GetY() {return m_yValue;}

	/// <summary>
	/// Adds transitions for X and Y coordinates.
	/// </summary>
	/// <remarks>
	/// Call this function to add the specified transitions to the internal list of transitions to be applied to animation variables for X and Y coordinates.
	/// When you add transitions, they are not applied immediately and stored in an internal list. Transitions are applied
	/// (added to a storyboard for a particular value) when you call CAnimationController::AnimateGroup.
	/// If you don't need to apply a transition to one of coordinates, you can pass NULL.
	/// </remarks>
	/// <param name="pXTransition">A pointer to transition for X coordinates.</param>
	/// <param name="pYTransition">A pointer to transition for Y coordinate.</param>
	void AddTransition(CBaseTransition* pXTransition, CBaseTransition* pYTransition);

protected:
	/// <summary>
	/// Puts the encapsulated animation variables into a list.
	/// </summary>
	/// <param name="lst">When the function returns, it contains pointers to two CAnimationVariable objects
	/// representing the X and Y coordinates.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst);
};

/// <summary>
/// Implements the functionality of a size object whose dimensions can be animated.
/// </summary>
/// <remarks>
/// The CAnimationSize class encapsulates two CAnimationVariable objects and can represent in applications
/// a size. For example, you can use this class to animate a size of any two dimensional object on the screen (like rectangle, control etc).
/// To use this class in application, just instantiate an object of this class, add it to animation controller using
/// CAnimationController::AddAnimationObject and call AddTransition for each transition to be applied to Width and/or Height.
/// </remarks>
class CAnimationSize  : public CAnimationBaseObject
{
	DECLARE_DYNAMIC(CAnimationSize)
protected:
	/// <summary>
	/// The encapsulated animation variable that represents width of animation size.
	/// </summary>
	CAnimationVariable 	m_cxValue;

	/// <summary>
	/// The encapsulated animation variable that represents height of animation size.
	/// </summary>
	CAnimationVariable 	m_cyValue;

public:
	/// <summary>
	/// Constructs an animation size object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with default values for width, height, Object ID and Group ID, which will be set to 0. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	CAnimationSize();

	/// <summary>
	/// Constructs an animation size object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with specified values for width, height, Object ID and Group ID. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	/// <param name="szDefault">Specifies default size.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationSize(const CSize& szDefault, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Sets default value.
	/// </summary>
	/// <remarks>
	/// Use this function to set a default value to animation object. This methods assigns default values to Width and Height
	/// of animation size. It also recreates underlying COM objects if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="szDefault">Specifies new default size.</param>
	void SetDefaultValue(const CSize& szDefault);

	/// <summary>
	/// Returns the default values for Width and Height.
	/// </summary>
	/// <returns>
	/// A CSize object containing default values.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve default value, which was previously set by constructor or SetDefaultValue.
	/// </remarks>
	CSize GetDefaultValue();

	/// <summary>
	/// Returns current value.
	/// </summary>
	/// <returns>
	/// TRUE, if the current value was successfully retrieved; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value of animation size. If this method fails or underlying COM objects for
	/// Width and Size have not been initialized, szValue contains default value, which was previously set in constructor or by SetDefaultValue.
	/// </remarks>
	/// <param name="szValue">Output. Contains the current value when this method returns.</param>
	BOOL GetValue(CSize& szValue);

	/// <summary>
	/// Provides access to CAnimationVariable representing Width.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Width.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Width.
	/// </remarks>
	CAnimationVariable& GetCX() {return m_cxValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing Height.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Height.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Height.
	/// </remarks>
	CAnimationVariable& GetCY() {return m_cyValue;}

	/// <summary>
	/// Assigns szSrc to CAnimationSize.
	/// </summary>
	/// <remarks>
	/// Assigns szSrc to CAnimationSize. It's recommended to do that before animation start, because this operator calls
	/// SetDefaultValue, which recreates the underlying COM objects for Width and Height if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="szSrc">Refers to CSize or SIZE.</param>
	void operator=(const CSize& szSrc)
	{
		SetDefaultValue(szSrc);
	}

	/// <summary>
	/// Converts a CAnimationSize to a CSize.
	/// </summary>
	/// <returns>
	/// Current value of animation size as CSize.
	/// </returns>
	/// <remarks>
	/// This function internally calls GetValue. If GetValue for some reason fails, the returned size will contain default values for Width and Height.
	/// </remarks>
	operator CSize()
	{
		CSize sz;
		GetValue(sz);
		return sz;
	}


	/// <summary>
	/// Adds transitions for Width and Height.
	/// </summary>
	/// <remarks>
	/// Call this function to add the specified transitions to the internal list of transitions to be applied to animation variables for Width and Height.
	/// When you add transitions, they are not applied immediately and stored in an internal list. Transitions are applied
	/// (added to a storyboard for a particular value) when you call CAnimationController::AnimateGroup.
	/// If you don't need to apply a transition to one of dimensions, you can pass NULL.
	/// </remarks>
	/// <param name="pCXTransition">A pointer to transition for Width.</param>
	/// <param name="pCYTransition">A pointer to transition for Height.</param>

	void AddTransition(CBaseTransition* pCXTransition, CBaseTransition* pCYTransition);

protected:
	/// <summary>
	/// Puts the encapsulated animation variables into a list.
	/// </summary>
	/// <param name="lst">When the function returns, it contains pointers to two CAnimationVariable objects
	/// representing the width and height.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst);
};

/// <summary>
/// Implements the functionality of a color whose red, green and blue components can be animated.
/// </summary>
/// <remarks>
/// The CAnimationColor class encapsulates three CAnimationVariable objects and can represent in applications
/// a color. For example, you can use this class to animate colors of any object on the screen (like text color, background color etc).
/// To use this class in application, just instantiate an object of this class, add it to animation controller using
/// CAnimationController::AddAnimationObject and call AddTransition for each transition to be applied to Red, Green and Blue components.
/// </remarks>
class CAnimationColor : public CAnimationBaseObject
{
	DECLARE_DYNAMIC(CAnimationColor)
protected:
	/// <summary>
	/// The encapsulated animation variable that represents Red component of animation color.
	/// </summary>
	CAnimationVariable 	m_rValue;

	/// <summary>
	/// The encapsulated animation variable that represents Green component of animation color.
	/// </summary>
	CAnimationVariable 	m_gValue;

	/// <summary>
	/// The encapsulated animation variable that represents Blue component of animation color.
	/// </summary>
	CAnimationVariable 	m_bValue;

public:
	/// <summary>
	/// Constructs a CAnimationColor object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with default values for red, green, blue, Object ID and Group ID, which will be set to 0. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	CAnimationColor();

	/// <summary>
	/// Constructs an animation color object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with specified values for RGB components, Object ID and Group ID. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	/// <param name="color">Specifies default color.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationColor(COLORREF color, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Sets default value.
	/// </summary>
	/// <remarks>
	/// Use this function to set a default value to animation object. This methods assigns default values to color components
	/// of animation color. It also recreates underlying COM objects if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="color">Specifies new default values for red, green and blue components.</param>
	void SetDefaultValue(COLORREF color);

	/// <summary>
	/// Returns the default values for color components.
	/// </summary>
	/// <returns>
	/// A COLORREF value containing defaults for RGB components.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve default value, which was previously set by constructor or SetDefaultValue.
	/// </remarks>
	COLORREF GetDefaultValue();

	/// <summary>
	/// Returns current value.
	/// </summary>
	/// <returns>
	/// TRUE, if the current value was successfully retrieved; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value of animation color. If this method fails or underlying COM objects for
	/// color components have not been initialized, color contains default value, which was previously set in constructor or by SetDefaultValue.
	/// </remarks>
	/// <param name="color">Output. Contains the current value when this method returns.</param>
	BOOL GetValue(COLORREF& color);

	/// <summary>
	/// Provides access to CAnimationVariable representing Red component.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Red component.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Red component.
	/// </remarks>
	CAnimationVariable& GetR() {return m_rValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing Green component.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Green component.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Green component.
	/// </remarks>
	CAnimationVariable& GetG() {return m_gValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing Blue component.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing Blue component.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing Blue component.
	/// </remarks>
	CAnimationVariable& GetB() {return m_bValue;}

	/// <summary>
	/// Assigns color to CAnimationColor.
	/// </summary>
	/// <remarks>
	/// It's recommended to do that before animation start, because this operator calls
	/// SetDefaultValue, which recreates the underlying COM objects for color components if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="color">Specifies new value Animation Color.</param>
	void operator=(COLORREF color)
	{
		SetDefaultValue(color);
	}

	/// <summary>
	/// Converts a CAnimationColor to COLORREF.
	/// </summary>
	/// <returns>
	/// Current value of animation color object as COLORREF.
	/// </returns>
	/// <remarks>
	/// This function internally calls GetValue. If GetValue for some reason fails, the returned COLORREF will contain default values for all color components.
	/// </remarks>
	operator COLORREF()
	{
		COLORREF color;
		GetValue(color);
		return color;
	}

	/// <summary>
	/// Adds transitions for Red, Green and Blue components.
	/// </summary>
	/// <remarks>
	/// Call this function to add the specified transitions to the internal list of transitions to be applied to animation variables representing color components.
	/// When you add transitions, they are not applied immediately and stored in an internal list. Transitions are applied
	/// (added to a storyboard for a particular value) when you call CAnimationController::AnimateGroup.
	/// If you don't need to apply a transition to one of the color components, you can pass NULL.
	/// </remarks>
	/// <param name="pRTransition">Transition for Red component.</param>
	/// <param name="pGTransition">Transition for Green component.</param>
	/// <param name="pBTransition">Transition for Blue component.</param>
	void AddTransition(CBaseTransition* pRTransition, CBaseTransition* pGTransition, CBaseTransition* pBTransition);

protected:
	/// <summary>
	/// Puts the encapsulated animation variables into a list.
	/// </summary>
	/// <param name="lst">When the function returns, it contains pointers to three CAnimationVariable objects
	/// representing red, green and blue components.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst);
};

/// <summary>
/// Implements the functionality of a rectangle whose top, left, right and bottom sides can be animated.
/// </summary>
/// <remarks>
/// The CAnimationRect class encapsulates four CAnimationVariable objects and can represent in applications
/// a rectangle.
/// To use this class in application, just instantiate an object of this class, add it to animation controller using
/// CAnimationController::AddAnimationObject and call AddTransition for each transition to be applied to left, right top and bottom coordinates.
/// </remarks>
class CAnimationRect : public CAnimationBaseObject
{
	DECLARE_DYNAMIC(CAnimationRect)
protected:
	/// <summary>
	/// The encapsulated animation variable that represents Left bound of animation rectangle.
	/// </summary>
	CAnimationVariable m_leftValue;

	/// <summary>
	/// The encapsulated animation variable that represents Top bound of animation rectangle.
	/// </summary>
	CAnimationVariable m_topValue;

	/// <summary>
	/// The encapsulated animation variable that represents Right bound of animation rectangle.
	/// </summary>
	CAnimationVariable m_rightValue;

	/// <summary>
	/// The encapsulated animation variable that represents Bottom bound of animation rectangle.
	/// </summary>
	CAnimationVariable m_bottomValue;

	/// <summary>
	/// Specifies initial size of animation rectangle.
	/// </summary>
	CSize 	m_szInitial;

public:
	/// <summary>
	/// Constructs a CAnimationRect object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with default values for left, top, right and bottom, Object ID and Group ID, which will be set to 0. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	CAnimationRect();

	/// <summary>
	/// Constructs an animation rect object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with specified rect coordinates, Object ID and Group ID. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	/// <param name="rect">Specifies default rectangle.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationRect(const CRect& rect, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Constructs an animation rect object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with specified top-left corner coordinates and size of rectangle, Object ID and Group ID. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	/// <param name="pt">Coordinate of top-left corner.</param>
	/// <param name="sz">Size of rectangle.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationRect(const CPoint& pt, const CSize& sz, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Constructs an animation rect object.
	/// </summary>
	/// <remarks>
	/// The object is constructed with specified coordinates of each side, Object ID and Group ID. They can be changed
	/// later at runtime using SetDefaultValue and SetID.
	/// </remarks>
	/// <param name="nLeft">Specifies coordinate of left bound.</param>
	/// <param name="nTop">Specifies coordinate of top bound.</param>
	/// <param name="nRight">Specifies coordinate of right bound.</param>
	/// <param name="nBottom">Specifies coordinate of bottom bound.</param>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="nObjectID">Specifies Object ID.</param>
	/// <param name="dwUserData">Specifies user-defined data.</param>
	CAnimationRect(int nLeft, int nTop, int nRight, int nBottom, UINT32 nGroupID, UINT32 nObjectID = (UINT32)-1, DWORD_PTR dwUserData = 0);

	/// <summary>
	/// Sets default value.
	/// </summary>
	/// <remarks>
	/// Use this function to set a default value to animation object. This methods assigns default values to rectangle's bounds.
	/// It also recreates underlying COM objects if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="rect">Specifies new default values for left, top, right and bottom.</param>
	void SetDefaultValue(const CRect& rect);

	/// <summary>
	/// Returns the default values for rectangle's bounds.
	/// </summary>
	/// <returns>
	/// A CRect value containing defaults for left, right, top and bottom.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve default value, which was previously set by constructor or SetDefaultValue.
	/// </remarks>
	CRect GetDefaultValue();

	/// <summary>
	/// Returns current value.
	/// </summary>
	/// <returns>
	/// TRUE, if the current value was successfully retrieved; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to retrieve the current value of animation rectangle. If this method fails or underlying COM objects for
	/// left, top, right and bottom have not been initialized, rect contains default value, which was previously set in constructor or by SetDefaultValue.
	/// </remarks>
	/// <param name="rect">Output. Contains the current value when this method returns.</param>
	BOOL GetValue(CRect& rect);

	/// <summary>
	/// Provides access to CAnimationVariable representing left coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing left coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing the left coordinate.
	/// </remarks>
	CAnimationVariable& GetLeft() 	{return m_leftValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing top coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing top coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing the top coordinate.
	/// </remarks>
	CAnimationVariable& GetTop() 	{return m_topValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing right coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing right coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing the right coordinate.
	/// </remarks>
	CAnimationVariable& GetRight() 	{return m_rightValue;}

	/// <summary>
	/// Provides access to CAnimationVariable representing bottom coordinate.
	/// </summary>
	/// <returns>
	/// A reference to encapsulated CAnimationVariable representing bottom coordinate.
	/// </returns>
	/// <remarks>
	/// You can call this method to get direct access to underlying CAnimationVariable representing the bottom coordinate.
	/// </remarks>
	CAnimationVariable& GetBottom() {return m_bottomValue;}

	/// <summary>
	/// Assigns rect to CAnimationRect.
	/// </summary>
	/// <remarks>
	/// It's recommended to do that before animation start, because this operator calls
	/// SetDefaultValue, which recreates the underlying COM objects for color components if they have been created. If you
	/// subscribed this animation object to events (ValueChanged or IntegerValueChanged), you need to re-enable these events.
	/// </remarks>
	/// <param name="rect">The new value of animation rectangle.</param>
	void operator=(const RECT& rect)
	{
		SetDefaultValue(rect);
	}

	/// <summary>
	/// Converts a CAnimationRect to RECT.
	/// </summary>
	/// <returns>
	/// Current value of animation rectangle as RECT.
	/// </returns>
	/// <remarks>
	/// This function internally calls GetValue. If GetValue for some reason fails, the returned RECT will contain default values for all rectangle coordinates.
	/// </remarks>
	operator RECT()
	{
		CRect rect;
		GetValue(rect);
		return rect;
	}

	/// <summary>
	/// Adds transitions for left, top, right and bottom coordinates.
	/// </summary>
	/// <remarks>
	/// Call this function to add the specified transitions to the internal list of transitions to be applied to animation variables for each rectangle sides.
	/// When you add transitions, they are not applied immediately and stored in an internal list. Transitions are applied
	/// (added to a storyboard for a particular value) when you call CAnimationController::AnimateGroup.
	/// If you don't need to apply a transition to one of the rectangle sides, you can pass NULL.
	/// </remarks>
	/// <param name="pLeftTransition">Specifies transition for the left side.</param>
	/// <param name="pTopTransition">Specifies transition for the top side.</param>
	/// <param name="pRightTransition">Specifies transition for the right side.</param>
	/// <param name="pBottomTransition">Specifies transition for the bottom side.</param>
	void AddTransition(CBaseTransition* pLeftTransition, CBaseTransition* pTopTransition,
		CBaseTransition* pRightTransition, CBaseTransition* pBottomTransition);

public:
	/// <summary>
	/// Specifies whether the rectangle has fixed size.
	/// </summary>
	/// <remarks>
	/// If this member is true, then the size of rectangle is fixed and right and bottom values
	/// are recalculated each time the top-left corner is  moved according to the fixed size.
	/// Set this value to TRUE to easily move the rectangle around the screen. In this case transitions applied to
	/// right and bottom coordinates are ignored. The size is stored internally when you construct the object and/or
	/// call SetDefaultValue. By default this member is set to FALSE.
	/// </remarks>
	BOOL 	m_bFixedSize;

protected:
	/// <summary>
	/// Puts the encapsulated animation variables into a list.
	/// </summary>
	/// <param name="lst">When the function returns, it contains pointers to four CAnimationVariable objects
	/// representing coordinates of rectangle.</param>
	virtual void GetAnimationVariableList(CList<CAnimationVariable*, CAnimationVariable*>& lst);
};

/// <summary>
/// Implements an animation group, which combines an animation storyboard, animation objects and transitions
/// to define an animation.
/// </summary>
/// <remarks>
/// Animation groups are created automatically by animation controller (CAnimationController) when you add animation
/// objects using CAnimationController::AddAnimationObject.
/// An animation group is identified by GroupID, which is usually taken as a parameter to manipulate animation groups.
/// The GroupID is taken from the first animation object being added to a new animation group.
/// An encapsulated animation storyboard is created after you call CAnimationController::AnimateGroup and can be
/// accessed via public member m_pStoryboard.
/// </remarks>
class CAnimationGroup
{
protected:
	/// <summary>
	/// A unique identifier of animation group.
	/// </summary>
	UINT32 	m_nGroupID;

	/// <summary>
	/// A pointer to animation controller this group belongs to.
	/// </summary>
	CAnimationController* m_pParentController;
protected:
	/// <summary>
	/// A helper that adds keyframes to a storyboard.
	/// </summary>
	/// <param name="pStoryboard">A pointer to a storyboard COM object.</param>
	/// <param name="bAddDeep">Specifies whether this method should add to the storyboard keyframes that depend on other keyframes.</param>
	void AddKeyframes(IUIAnimationStoryboard* pStoryboard, BOOL bAddDeep);

	/// <summary>
	/// A helper that adds transitions to a storyboard.
	/// </summary>
	/// <param name="pStoryboard">A pointer to a storyboard COM object.</param>
	/// <param name="bDependOnKeyframes">Specifies whether this method should add to the storyboard transitions that depend on other keyframes.</param>
	void AddTransitions(IUIAnimationStoryboard* pStoryboard, BOOL bDependOnKeyframes);

	/// <summary>
	/// A helper that creates COM transition objects.
	/// </summary>
	/// <returns>
	/// TRUE is the method succeeds, otherwise FALSE.
	/// </returns>
	BOOL CreateTransitions();

public:
	/// <summary>
	/// Constructs an animation group.
	/// </summary>
	/// <param name="pParentController">A pointer to animation controller that creates a group.</param>
	/// <param name="nGroupID">Specifies GroupID.</param>
	CAnimationGroup(CAnimationController* pParentController, UINT32 nGroupID);

	/// <summary>
	/// The destructor. Called when an animation group is being destroyed.
	/// </summary>
	~CAnimationGroup();

	/// <summary>
	/// Returns GroupID.
	/// </summary>
	/// <returns>
	/// A group identifier.
	/// </returns>
	UINT32 GetGroupID() const {return m_nGroupID;}

	/// <summary>
	/// Applies transitions to animation objects.
	/// </summary>
	/// <remarks>
	/// This method ASSERTS in debug mode if storyboard has not been created.
	/// It creates all transitions first, then adds "static" keyframes (keyframes that depend on offsets),
	/// adds transitions that do not depend on keyframes, adds keyframes depending on transitions and other keyframes,
	/// and at last adds transitions that depend on keyframes.
	/// </remarks>
	void ApplyTransitions();

	/// <summary>
	/// Removes transitions from animation objects that belong to an animation group.
	/// </summary>
	/// <remarks>
	/// If m_bAutoclearTransitions flag is set to TRUE, this method loops over all animation objects that belong to the group
	/// and calls CAnimationObject::ClearTransitions(FALSE).
	/// </remarks>
	void RemoveTransitions();

	/// <summary>
	/// Removes and optionally destroys all keyframes that belong to an animation group.
	/// </summary>
	/// <remarks>
	/// If m_bAutodestroyKeyframes member is TRUE then keyframes are removed and destroyed, otherwise keyframes are
	/// just removed from the internal list of keyframes.
	/// </remarks>
	void RemoveKeyframes();

	/// <summary>
	/// Finds an animation object that contains the specified animation variable.
	/// </summary>
	/// <returns>
	/// A pointer to animation object, or NULL if animation object is not found.
	/// </returns>
	/// <param name="pVariable">A pointer to animation variable.</param>
	CAnimationBaseObject* FindAnimationObject(IUIAnimationVariable* pVariable);

	/// <summary>
	/// Animates a group.
	/// </summary>
	/// <returns>
	/// TRUE if the method succeeds; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method creates an internal storyboard, creates and applies transitions and schedules an animation
	/// if bScheduleNow is TRUE. If bScheduleNow is FALSE, you need to call Schedule to start animation at the
	/// specified time.
	/// </remarks>
	/// <param name="pManager"></param>
	/// <param name="pTimer"></param>
	/// <param name="bScheduleNow"></param>
	BOOL Animate(IUIAnimationManager* pManager, IUIAnimationTimer* pTimer, BOOL bScheduleNow);

	/// <summary>
	/// Schedules an animation at the specified time.
	/// </summary>
	/// <returns>
	/// TRUE if the method succeeds; FALSE if the method fails or if Animate has not been called with bScheduleNow set to FALSE.
	/// </returns>
	/// <remarks>
	/// Call this function to schedule an animation at the specified time. You must call Animate with bScheduleNow set to FALSE first.
	/// </remarks>
	/// <param name="pTimer">A pointer to animation timer.</param>
	/// <param name="time">Specifies time to schedule the animation.</param>
	BOOL Schedule(IUIAnimationTimer* pTimer, UI_ANIMATION_SECONDS time);

	/// <summary>
	/// Directs all animation objects that belong to group automatically destroy transitions.
	/// </summary>
	/// <remarks>
	/// Set this value to FALSE only if you allocate transitions on the stack. The default value is TRUE, therefore
	/// it's highly recommended to allocate transition objects using operator new.
	/// </remarks>
	/// <param name="bAutoDestroy">Specifies how to destroy transitions.</param>
	void SetAutodestroyTransitions(BOOL bAutoDestroy = TRUE);

public:
	/// <summary>
	/// Contains a list of animation objects.
	/// </summary>
	CObList m_lstAnimationObjects;

	/// <summary>
	/// Contains a list of keyframes.
	/// </summary>
	CObList m_lstKeyFrames;

	/// <summary>
	/// Points to animation storyboard. This pointer is valid only after call on Animate.
	/// </summary>
	ATL::CComPtr<IUIAnimationStoryboard> m_pStoryboard;

	/// <summary>
	/// Specifies how to destroy keyframes. If this value is TRUE, all keyframes are removed and destroyed;
	/// otherwise they are removed from the list only. The default value is TRUE.
	/// </summary>
	BOOL	m_bAutodestroyKeyframes;

	/// <summary>
	/// Specifies how to clear transitions from animation objects that belong to group. If this member is TRUE,
	/// transitions are removed automatically when an animation has been scheduled. Otherwise you need to remove
	/// transitions manually.
	/// </summary>
	BOOL 	m_bAutoclearTransitions;

	/// <summary>
	/// Specifies how to destroy animation objects. If this parameter is TRUE, animation objects will be destroyed automatically
	/// when the group is destroyed. Otherwise animation objects must be destroyed manually. The default value is FALSE.
	/// Set this value to TRUE only if all animation objects that belong to group are allocated dynamically with operator new.
	/// </summary>
	BOOL 	m_bAutodestroyAnimationObjects;
};

// PHT - priority handler type
#define UI_ANIMATION_PHT_NONE				0x0000
#define UI_ANIMATION_PHT_CANCEL 			0x0001
#define UI_ANIMATION_PHT_CONCLUDE 			0x0002
#define UI_ANIMATION_PHT_COMPRESS			0x0004
#define UI_ANIMATION_PHT_TRIM 				0x0008
#define UI_ANIMATION_PHT_ALL 				0x00FF
#define UI_ANIMATION_PHT_CANCEL_REMOVE 		0x0010
#define UI_ANIMATION_PHT_CONCLUDE_REMOVE	0x0020
#define UI_ANIMATION_PHT_COMPRESS_REMOVE	0x0040
#define UI_ANIMATION_PHT_TRIM_REMOVE		0x0080

/// <summary>
/// Implements the animation controller, which provides a central interface for creating and managing UI animations.
/// </summary>
/// <remarks>
/// The CAnimationController class is the key class that manages animations.
/// You may create one or more instances of animation controller in an application and, optionally, connect an instance of
/// animation controller to a CWnd object using CAnimationController::SetRelatedWnd. This connection is required to send WM_PAINT messages
/// to the related window automatically when animation manager status has changed or animation timer has been updated.
/// If you do not enable this relation, you must redraw a window that displays an animation manually. For this purpose you can derive a class
/// from CAnimationController and override OnAnimationManagerStatusChanged and/or OnAnimationTimerPostUpdate and invalidate one or
/// more windows when necessary.
/// </remarks>
class CAnimationController : public CObject
{
	DECLARE_DYNCREATE(CAnimationController)
protected:
	/// <summary>
	/// Specifies whether an animation controller is valid or not.
	/// This member is set to FALSE if current OS does not support Windows Animation API.
	/// </summary>
	BOOL m_bIsValid;

	/// <summary>
	/// Stores a pointer to Animation Manager COM object.
	/// </summary>
	ATL::CComPtr<IUIAnimationManager> m_pAnimationManager;

	/// <summary>
	/// Stores a pointer to Transition Library COM object.
	/// </summary>
	ATL::CComPtr<IUIAnimationTransitionLibrary> m_pTransitionLibrary;

	/// <summary>
	/// Stores a pointer to Animation Timer COM object.
	/// </summary>
	ATL::CComPtr<IUIAnimationTimer> m_pAnimationTimer;

	/// <summary>
	/// Stores a pointer to Transition Factory COM object.
	/// </summary>
	ATL::CComPtr<IUIAnimationTransitionFactory> m_pTransitionFactory;

	/// <summary>
	/// A pointer to a related CWnd object, which can be automatically redrawn when
	/// the status of animation manager has changed, or post update event has occurred. Can be NULL.
	/// </summary>
	CWnd* m_pRelatedWnd;

	/// <summary>
	/// A list of animation groups that belong to this animation controller.
	/// </summary>
	CList<CAnimationGroup*, CAnimationGroup*> m_lstAnimationGroups;

	/// <summary>
	/// A keyframe that represents start of storyboard.
	/// </summary>
	static CBaseKeyFrame g_KeyframeStoryboardStart;
public:

	/// <summary>
	/// Constructs an animation controller.
	/// </summary>
	CAnimationController(void);

	/// <summary>
	/// The destructor. Called when animation controller object is being destroyed.
	/// </summary>
	virtual ~CAnimationController(void);

	/// <summary>
	/// Provides access to encapsulated IUIAnimationManager object.
	/// </summary>
	/// <returns>
	/// A pointer to IUIAnimationManager interface or NULL, if creation of animation manager failed.
	/// </returns>
	/// <remarks>
	/// If current OS does not support Windows Animation API, this method returns NULL and after that all subsequent calls
	/// on CAnimationController::IsValid return FALSE.
	/// You may need to access IUIAnimationManager in order to call its interface methods, which are not wrapped by
	/// animation controller.
	/// </remarks>
	IUIAnimationManager* GetUIAnimationManager();

	/// <summary>
	/// Provides access to encapsulated IUIAnimationTransitionLibrary object.
	/// </summary>
	/// <returns>
	/// A pointer to IUIAnimationTransitionLibrary interface or NULL, if creation of transition library failed.
	/// </returns>
	/// <remarks>
	/// If current OS does not support Windows Animation API, this method returns NULL and after that all subsequent calls
	/// on CAnimationController::IsValid return FALSE.
	/// </remarks>
	IUIAnimationTransitionLibrary* GetUITransitionLibrary();

	/// <summary>
	/// Provides access to encapsulated IUIAnimationTimer object.
	/// </summary>
	/// <returns>
	/// A pointer to IUIAnimationTimer interface or NULL, if creation of animation timer failed.
	/// </returns>
	/// <remarks>
	/// If current OS does not support Windows Animation API, this method returns NULL and after that all subsequent calls
	/// on CAnimationController::IsValid return FALSE.
	/// </remarks>
	IUIAnimationTimer* GetUIAnimationTimer();

	/// <summary>
	/// A pointer to IUIAnimationTransitionFactory interface or NULL, if creation of transition library failed.
	/// </summary>
	/// <returns>
	/// A pointer to IUIAnimationTransitionFactory or NULL, if creation of transition factory failed.
	/// </returns>
	/// <remarks>
	/// If current OS does not support Windows Animation API, this method returns NULL and after that all subsequent calls
	/// on CAnimationController::IsValid return FALSE.
	/// </remarks>
	IUIAnimationTransitionFactory* GetUITransitionFactory();

	/// <summary>
	/// Tells whether animation controller is valid.
	/// </summary>
	/// <returns>
	/// TRUE if animation controller is valid; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// This method returns FALSE only if Windows Animation API is not supported on the current OS and creation of
	/// animation manager failed because it's not registered. You need to call GetUIAnimationManager at least once after initialization of
	/// COM libraries to cause setting of this flag.
	/// </remarks>
	BOOL IsValid() const {return m_bIsValid;}

	/// <summary>
	/// Returns a keyframe that identifies start of storyboard.
	/// </summary>
	/// <returns>
	/// A pointer to base keyframe, which identifies start of storyboard.
	/// </returns>
	/// <remarks>
	/// Obtain this keyframe to base any other keyframes or transitions on the moment in time when a storyboard starts.
	/// </remarks>
	static CBaseKeyFrame* GetKeyframeStoryboardStart();

	/// <summary>
	/// Sets or releases a handler to call when animation manager's status changes.
	/// </summary>
	/// <returns>
	/// TRUE if the handler was successfully set or released.
	/// </returns>
	/// <remarks>
	/// When a handler is set (enabled) Windows Animation calls OnAnimationManagerStatusChanged when animation manager's status changes.
	/// </remarks>
	/// <param name="bEnable">Specifies whether to set or release a handler.</param>
	virtual BOOL EnableAnimationManagerEvent(BOOL bEnable = TRUE);

	/// <summary>
	/// Sets or releases the priority comparison handler to call to determine whether a scheduled storyboard can be cancelled,
	/// concluded, trimmed or compressed.
	/// </summary>
	/// <returns>
	/// TRUE if the handler was successfully set or released.
	/// </returns>
	/// <remarks>
	/// When a handler is set (enabled) Windows Animation calls the following virtual methods depending on dwHandlerType:
	/// OnHasPriorityCancel, OnHasPriorityConclude, OnHasPriorityTrim, OnHasPriorityCompress.
	/// dwHandler can be a combination of the following flags:
	/// UI_ANIMATION_PHT_NONE - release all handlers
	/// UI_ANIMATION_PHT_CANCEL - set Cancel comparison handler
	/// UI_ANIMATION_PHT_CONCLUDE - set Conclude comparison handler
	/// UI_ANIMATION_PHT_COMPRESS - set Compress comparison handler
	/// UI_ANIMATION_PHT_TRIM - set Trim comparison handler
	/// UI_ANIMATION_PHT_CANCEL_REMOVE - remove Cancel comparison handler
	/// UI_ANIMATION_PHT_CONCLUDE_REMOVE - remove Conclude comparison handler
	/// UI_ANIMATION_PHT_COMPRESS_REMOVE - remove Compress comparison handler
	/// UI_ANIMATION_PHT_TRIM_REMOVE - remove Trim comparison handler
	/// </remarks>
	/// <param name="dwHandlerType">A combination of UI_ANIMATION_PHT_ flags (see remarks), which specifies what handlers to set or release.</param>
	virtual BOOL EnablePriorityComparisonHandler(DWORD_PTR dwHandlerType);

	/// <summary>
	/// Sets or releases a handler for storyboard status and update events.
	/// </summary>
	/// <returns>
	/// TRUE if the handler was successfully set or released; FALSE if the specified animation group is now found or
	/// animation for the specified group has not been initiated and its internal storyboard is NULL.
	/// </returns>
	/// <remarks>
	/// When a handler is set (enabled) Windows Animation API calls OnStoryboardStatusChanges and OnStoryboardUpdated virtual methods.
	/// A handler must be set after CAnimationController::Animate has been called for the specified animation group, because
	/// it creates encapsulated IUIAnimationStoryboard object.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="bEnable">Specifies whether to set or release a handler.</param>
	virtual BOOL EnableStoryboardEventHandler(UINT32 nGroupID, BOOL bEnable = TRUE);

	/// <summary>
	/// Sets or releases a handler for timing events and handler for timing updates.
	/// </summary>
	/// <returns>
	/// TRUE if handlers were successfully set or released; FALSE if this method is called for a second time without
	/// releasing the handlers first, or if any other error occurs.
	/// </returns>
	/// <remarks>
	/// When the handlers are set (enabled) Windows Animation API calls OnAnimationTimerPreUpdate, OnAnimationTimerPostUpdate,
	/// OnRenderingTooSlow methods.
	/// You need to enable animation timers to allow Windows Animation API update storyboards. Otherwise you'll need to
	/// call CAnimationController::UpdateAnimationManager in order to direct the animation manager to update the values
	/// of all animation variables.
	/// </remarks>
	/// <param name="bEnable">Specifies whether to set or release the handlers.</param>
	/// <param name="idleBehavior">Specifies idle behavior for timer update handler.</param>
	virtual BOOL EnableAnimationTimerEventHandler(BOOL bEnable = TRUE, UI_ANIMATION_IDLE_BEHAVIOR idleBehavior = UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);

	/// <summary>
	/// Directs the animation manager to update the values of all animation variables.
	/// </summary>
	/// <remarks>
	/// Calling this method advances the animation manager to current time, changing statuses of storyboards as necessary
	/// and updating any animation variables to appropriate interpolated values. Internally this method calls
	/// IUIAnimationTimer::GetTime(timeNow) and IUIAnimationManager::Update(timeNow). Override this method in a derived
	/// class to customize this behavior.
	/// </remarks>
	virtual void UpdateAnimationManager();

	/// <summary>
	/// Tells whether at least one group is playing animation.
	/// </summary>
	/// <returns>
	/// TRUE if there is an animation in progress for this animation controller; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Checks status of animation manager and returns TRUE if the status is UI_ANIMATION_MANAGER_BUSY.
	/// </remarks>
	virtual BOOL IsAnimationInProgress();

	/// <summary>
	/// Establishes a relationship between animation controller and a window.
	/// </summary>
	/// <remarks>
	/// If a related CWnd object is set, the animation controller can automatically update it (send WM_PAINT message) when
	/// the status of animation manager has changed or timer post update event has occurred.
	/// </remarks>
	/// <param name="pWnd">A pointer to window object to set.</param>
	void SetRelatedWnd(CWnd* pWnd)
	{
		m_pRelatedWnd = pWnd;
	}

	/// <summary>
	/// Adds an animation object to a group that belongs to the animation controller.
	/// </summary>
	/// <returns>
	/// A pointer to existing or new animation group where pObject has been added if function succeeds; NULL if
	/// pObject has already been added to a group that belongs to another animation controller.
	/// </returns>
	/// <remarks>
	/// Call this method to add an animation object to the animation controller. An object will be added to a group
	/// according to object's GroupID (see CAnimationBaseObject::SetID). The animation controller will create a new
	/// group if it's the first object being added with the specified GroupID. An animation object can be added to
	/// one animation controller only. If you need to add an object to another controller, call RemoveAnimationObject first.
	/// If you call SetID with new GroupID for an object that has been already added to a group, the object will be removed
	/// from the old group and added to another group with specified ID.
	/// </remarks>
	/// <param name="pObject">A pointer to an animation object.</param>
	CAnimationGroup* AddAnimationObject(CAnimationBaseObject* pObject);

	/// <summary>
	/// Remove an animation object from animation controller.
	/// </summary>
	/// <remarks>
	/// Removes an animation object from animation controller and animation group. Call this function if
	/// a particular object should not be animated anymore, or if you need to move the object to another animation controller.
	/// In the last case bNoDelete must be TRUE.
	/// </remarks>
	/// <param name="pObject">A pointer to an animation object.</param>
	/// <param name="bNoDelete">If this parameter is TRUE the object will not be deleted upon remove.</param>
	void RemoveAnimationObject(CAnimationBaseObject* pObject, BOOL bNoDelete = FALSE);

	/// <summary>
	/// Removes an animation group with specified ID from animation controller.
	/// </summary>
	/// <remarks>
	/// This method removes an animation group from the internal list of groups and deletes it, therefore if you stored
	/// a pointer to that animation group, it must be invalidated. If CAnimationGroup::m_bAutodestroyAnimationObjects is TRUE,
	/// all animation objects that belong to that group will be deleted; otherwise their references to parent animation
	/// controller will be set to NULL and they can be added to another controller.
	/// </remarks>
	/// <param name="nGroupID">Specifies animation group ID.</param>
	void RemoveAnimationGroup(UINT32 nGroupID);

	/// <summary>
	/// Removes all animation groups from animation controller.
	/// </summary>
	/// <remarks>
	/// All groups will be deleted, their pointer, if stored at the application level, must be invalidated.
	/// If CAnimationGroup::m_bAutodestroyAnimationObjects for a group being deleted is TRUE,
	/// all animation objects that belong to that group will be deleted; otherwise their references to parent animation
	/// controller will be set to NULL and they can be added to another controller.
	/// </remarks>
	void RemoveAllAnimationGroups();

	/// <summary>
	/// Removes transitions from animation objects that belong to the specified group.
	/// </summary>
	/// <remarks>
	/// The group loops over its animation objects and calls ClearTransitions(FALSE) for each animation object.
	/// This method is called by the framework after animation has been scheduled.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID.</param>
	void RemoveTransitions(UINT32 nGroupID);

	/// <summary>
	/// Creates a keyframe that depends on transition and adds it to the specified group.
	/// </summary>
	/// <returns>
	/// A pointer to newly created keyframe if the function succeeds.
	/// </returns>
	/// <remarks>
	/// You can store the returned pointer and base other keyframes on the newly created keyframe (see the second overload).
	/// It's possible to begin transitions at keyframes - see CBaseTransition::SetKeyframes.
	/// You don't need to delete keyframes created in this way, because they are deleted automatically by animation groups.
	/// Be careful when creating keyframes based on other keyframes and transitions and avoid circular references.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID for which keyframe is created.</param>
	/// <param name="pTransition">A pointer to transition. Keyframe will be inserted to storyboard after this transition.</param>
	CKeyFrame* CreateKeyframe(UINT32 nGroupID, CBaseTransition* pTransition);

	/// <summary>
	/// Creates a keyframe that depends on other keyframe with optional offset in seconds and adds it to the specified group.
	/// </summary>
	/// <returns>
	/// A pointer to newly created keyframe if the function succeeds.
	/// </returns>
	/// <remarks>
	/// You can store the returned pointer and base other keyframes on the newly created keyframe (see the second overload).
	/// It's possible to begin transitions at keyframes - see CBaseTransition::SetKeyframes.
	/// You don't need to delete keyframes created in this way, because they are deleted automatically by animation groups.
	/// Be careful when creating keyframes based on other keyframes and transitions and avoid circular references.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID for which keyframe is created.</param>
	/// <param name="pKeyframe">A  pointer to base keyframe for this keyframe.</param>
	/// <param name="offset">Offset in seconds from the base keyframe specified by pKeyframe.</param>
	CKeyFrame* CreateKeyframe(UINT32 nGroupID, CBaseKeyFrame* pKeyframe, UI_ANIMATION_SECONDS offset = 0.0);

	/// <summary>
	/// Adds a keyframe to group.
	/// </summary>
	/// <returns>
	/// TRUE if the function succeeds; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Usually you don't need to call this method, use CAnimationController::CreateKeyframe instead, which creates
	/// and adds the created keyframe to a group automatically.
	/// </remarks>
	/// <param name="nGroupID">Specifies Group ID.</param>
	/// <param name="pKeyframe">A pointer to a keyframe.</param>
	BOOL AddKeyframeToGroup(UINT32 nGroupID, CBaseKeyFrame* pKeyframe);

	/// <summary>
	/// Finds an animation group by its Group ID.
	/// </summary>
	/// <returns>
	/// A pointer to animation group or NULL if the group with specified ID is not found.
	/// </returns>
	/// <remarks>
	/// Use this method to find an animation group at runtime. A group is created and added to the internal list of animation groups
	/// when a first animation object with particular GroupID is being added to animation controller.
	/// </remarks>
	/// <param name="nGroupID">Specifies a GroupID.</param>
	CAnimationGroup* FindAnimationGroup(UINT32 nGroupID);

	/// <summary>
	/// Finds an animation group by its storyboard.
	/// </summary>
	/// <returns>
	/// A pointer to animation group if succeeds, or NULL if no group plays the specified storyboard.
	/// </returns>
	/// <remarks>
	/// This method is usually called from event handlers to find a group by pointer to storyboard that comes as a parameter to an event handler.
	/// </remarks>
	/// <param name="pStoryboard">A pointer to a storyboard.</param>
	CAnimationGroup* FindAnimationGroup(IUIAnimationStoryboard* pStoryboard);

	/// <summary>
	/// Finds animation object containing a specified animation variable.
	/// </summary>
	/// <returns>
	/// TRUE if object was found; otherwise FALSE.
	/// </returns>
	/// <remarks>
	/// Called from event handlers when it's required to find an animation object from incoming animation variable.
	/// </remarks>
	/// <param name="pVariable">A pointer to animation variable.</param>
	/// <param name="ppObject">Output. Contains a pointer to animation object or NULL. </param>
	/// <param name="ppGroup">Output. Contains a pointer to animation group that holds the animation object, or NULL.</param>
	BOOL FindAnimationObject(IUIAnimationVariable* pVariable, CAnimationBaseObject** ppObject, CAnimationGroup** ppGroup);

	/// <summary>
	/// Prepares a group to run animation and optionally schedules it.
	/// </summary>
	/// <returns>
	/// TRUE if animation was successfully scheduled and run.
	/// </returns>
	/// <remarks>
	/// This method does the actual work creating storyboard, adding animation variables, applying transitions and setting keyframes.
	/// It's possible to delay scheduling if you set bScheduleNow to FALSE. In this case the specified group
	/// will hold a storyboard that has been set up for animation. At that point you can setup events for the storyboard and
	/// animation variables. When you actually need to run the animation call CAnimationController::ScheduleGroup.
	/// </remarks>
	/// <param name="nGroupID">Specifies GroupID.</param>
	/// <param name="bScheduleNow">Specifies whether to run animation right away.</param>
	BOOL AnimateGroup(UINT32 nGroupID, BOOL bScheduleNow = TRUE);

	/// <summary>
	/// Schedules an animation.
	/// </summary>
	/// <returns>
	/// TRUE if animation was scheduled successfully. FALSE if storyboard has not been created, or other error occurs.
	/// </returns>
	/// <remarks>
	/// You must call AnimateGroup with parameter bScheduleNow set to FALSE prior ScheduleGroup. You can specify the
	/// desired animation time obtained from IUIAnimationTimer::GetTime. If the time parameter is 0.0, the animation is scheduled for the
	/// current time.
	/// </remarks>
	/// <param name="nGroupID">Specifies animation Group ID to schedule.</param>
	/// <param name="time">Specifies time to schedule.</param>
	BOOL ScheduleGroup(UINT32 nGroupID, UI_ANIMATION_SECONDS time = 0.0);

	/// <summary>
	/// Called by the framework to clean up the group when animation has been scheduled.
	/// </summary>
	/// <remarks>
	/// This method removes all transitions and keyframes from the specified group, because they are not relevant
	/// after an animation has been scheduled.
	/// </remarks>
	/// <param name="nGroupID">Specifies GroupID.</param>
	void CleanUpGroup(UINT32 nGroupID);

	/// <summary>
	/// Called by the framework right before the animation is scheduled.
	/// </summary>
	/// <remarks>
	/// This call is routed to related CWnd and can be overridden in a derived class to perform any
	/// additional actions before the animation starts for the specified group.
	/// </remarks>
	/// <param name="pGroup">A pointer to an animation group whose animation is about to start.</param>
	virtual void OnBeforeAnimationStart(CAnimationGroup* pGroup);

	/// <summary>
	/// Called by the framework in response to StatusChanged event from animation manager.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable animation manager events with EnableAnimationManagerEvent.
	/// It can be overridden in a derived class to take application-specific actions. The default implementation
	/// updates a related window if it has been set with SetRelatedWnd.
	/// </remarks>
	/// <param name="newStatus">New animation manager status.</param>
	/// <param name="previousStatus">Previous animation manager status.</param>
	virtual void OnAnimationManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus, UI_ANIMATION_MANAGER_STATUS previousStatus);

	/// <summary>
	/// Called by the framework when value of animation variable has changed.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable animation variable events with EnableValueChangedEvent called for a specific
	/// animation variable or animation object.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	/// <param name="pGroup">A pointer to an animation group that holds an animation object whose value has changed.</param>
	/// <param name="pObject">A pointer to an animation object that contains an animation variable whose value has changed.</param>
	/// <param name="variable">A pointer to an animation variable.</param>
	/// <param name="newValue">Specifies new value.</param>
	/// <param name="prevValue">Specifies previous value.</param>
	virtual void OnAnimationValueChanged(CAnimationGroup* pGroup, CAnimationBaseObject* pObject, IUIAnimationVariable* variable, DOUBLE newValue, DOUBLE prevValue);

	/// <summary>
	/// Called by the framework when integer value of animation variable has changed.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable animation variable events with EnableIntegerValueChangedEvent called for a specific
	/// animation variable or animation object.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	/// <param name="pGroup">A pointer to an animation group that holds an animation object whose value has changed.</param>
	/// <param name="pObject">A pointer to an animation object that contains an animation variable whose value has changed.</param>
	/// <param name="variable">A pointer to an animation variable.</param>
	/// <param name="newValue">Specifies new value.</param>
	/// <param name="prevValue">Specifies previous value.</param>
	virtual void OnAnimationIntegerValueChanged(CAnimationGroup* pGroup, CAnimationBaseObject* pObject, IUIAnimationVariable* variable, INT32 newValue, INT32 prevValue);

	/// <summary>
	/// Called by the framework when storyboard status has changed.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable storyboard events using CAnimationController::EnableStoryboardEventHandler.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	/// <param name="pGroup">A pointer to an animation group that owns the storyboard whose status has changed.</param>
	/// <param name="newStatus">Specifies the new status.</param>
	/// <param name="previousStatus">Specifies the previous status.</param>
	virtual void OnStoryboardStatusChanged(CAnimationGroup* pGroup, UI_ANIMATION_STORYBOARD_STATUS newStatus, UI_ANIMATION_STORYBOARD_STATUS previousStatus);

	/// <summary>
	/// Called by the framework when storyboard has been updated.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable storyboard events using CAnimationController::EnableStoryboardEventHandler.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	/// <param name="pGroup">A pointer to a group that owns the storyboard.</param>
	virtual void OnStoryboardUpdated(CAnimationGroup* pGroup);

	/// <summary>
	/// Called by the framework to resolve scheduling conflicts.
	/// </summary>
	/// <returns>
	/// Should return TRUE if storyboard owned by pGroupNew has priority. Should return FALSE if storyboard owned by
	/// pGroupScheduled has priority.
	/// </returns>
	/// <remarks>
	/// This method is called if you enable priority comparison events using CAnimationController::EnablePriorityComparisonHandler
	/// and specify UI_ANIMATION_PHT_CANCEL.
	/// It can be overridden in a derived class to take application-specific actions.
	/// Read Windows Animation API documentation for more information about Conflict Management (http://msdn.microsoft.com/en-us/library/dd371759(VS.85).aspx).
	/// </remarks>
	/// <param name="pGroupScheduled">The group that owns the currently scheduled storyboard.</param>
	/// <param name="pGroupNew">The group that owns the new storyboard that is in scheduling conflict with the scheduled storyboard owned by pGroupScheduled.</param>
	/// <param name="priorityEffect">The potential effect on pGroupNew if pGroupScheduled has a higher priority.</param>
	virtual BOOL OnHasPriorityCancel(CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect);

	/// <summary>
	/// Called by the framework to resolve scheduling conflicts.
	/// </summary>
	/// <returns>
	/// Should return TRUE if storyboard owned by pGroupNew has priority. Should return FALSE if storyboard owned by
	/// pGroupScheduled has priority.
	/// </returns>
	/// <remarks>
	/// This method is called if you enable priority comparison events using CAnimationController::EnablePriorityComparisonHandler
	/// and specify UI_ANIMATION_PHT_CONCLUDE.
	/// It can be overridden in a derived class to take application-specific actions.
	/// Read Windows Animation API documentation for more information about Conflict Management (http://msdn.microsoft.com/en-us/library/dd371759(VS.85).aspx).
	/// </remarks>
	/// <param name="pGroupScheduled">The group that owns the currently scheduled storyboard.</param>
	/// <param name="pGroupNew">The group that owns the new storyboard that is in scheduling conflict with the scheduled storyboard owned by pGroupScheduled.</param>
	/// <param name="priorityEffect">The potential effect on pGroupNew if pGroupScheduled has a higher priority.</param>
	virtual BOOL OnHasPriorityConclude(CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect);

	/// <summary>
	/// Called by the framework to resolve scheduling conflicts.
	/// </summary>
	/// <returns>
	/// Should return TRUE if storyboard owned by pGroupNew has priority. Should return FALSE if storyboard owned by
	/// pGroupScheduled has priority.
	/// </returns>
	/// <remarks>
	/// This method is called if you enable priority comparison events using CAnimationController::EnablePriorityComparisonHandler
	/// and specify UI_ANIMATION_PHT_TRIM.
	/// It can be overridden in a derived class to take application-specific actions.
	/// Read Windows Animation API documentation for more information about Conflict Management (http://msdn.microsoft.com/en-us/library/dd371759(VS.85).aspx).
	/// </remarks>
	/// <param name="pGroupScheduled">The group that owns the currently scheduled storyboard.</param>
	/// <param name="pGroupNew">The group that owns the new storyboard that is in scheduling conflict with the scheduled storyboard owned by pGroupScheduled.</param>
	/// <param name="priorityEffect">The potential effect on pGroupNew if pGroupScheduled has a higher priority.</param>
	virtual BOOL OnHasPriorityTrim(CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect);

	/// <summary>
	/// Called by the framework to resolve scheduling conflicts.
	/// </summary>
	/// <returns>
	/// Should return TRUE if storyboard owned by pGroupNew has priority. Should return FALSE if storyboard owned by
	/// pGroupScheduled has priority.
	/// </returns>
	/// <remarks>
	/// This method is called if you enable priority comparison events using CAnimationController::EnablePriorityComparisonHandler
	/// and specify UI_ANIMATION_PHT_COMPRESS.
	/// It can be overridden in a derived class to take application-specific actions.
	/// Read Windows Animation API documentation for more information about Conflict Management (http://msdn.microsoft.com/en-us/library/dd371759(VS.85).aspx).
	/// </remarks>
	/// <param name="pGroupScheduled">The group that owns the currently scheduled storyboard.</param>
	/// <param name="pGroupNew">The group that owns the new storyboard that is in scheduling conflict with the scheduled storyboard owned by pGroupScheduled.</param>
	/// <param name="priorityEffect">The potential effect on pGroupNew if pGroupScheduled has a higher priority.</param>
	virtual BOOL OnHasPriorityCompress(CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect);

	/// <summary>
	/// Called by the framework before an animation update begins.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable timer event handlers using EnableAnimationTimerEventHandler.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	virtual void OnAnimationTimerPreUpdate();

	/// <summary>
	/// Called by the framework after an animation update is finished.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable timer event handlers using EnableAnimationTimerEventHandler.
	/// It can be overridden in a derived class to take application-specific actions.
	/// </remarks>
	virtual void OnAnimationTimerPostUpdate();

	/// <summary>
	/// Called by the framework when the rendering frame rate for an animation falls below a minimum desirable frame rate.
	/// </summary>
	/// <remarks>
	/// This method is called if you enable timer event handlers using EnableAnimationTimerEventHandler.
	/// It can be overridden in a derived class to take application-specific actions.
	/// The minimum desirable frame rate is specified by calling IUIAnimationTimer::SetFrameRateThreshold.
	/// </remarks>
	/// <param name="fps">The current frame rate in frames per second. </param>
	virtual void OnAnimationTimerRenderingTooSlow(UINT32 fps);

protected:

	/// <summary>
	/// Called by the framework when an animation for the specified group has just been scheduled.
	/// </summary>
	/// <remarks>
	/// The default implementation removes keyframes from the specified group and transitions from animation variables that
	/// belong to the specified group. Can be overridden in a derived class to take any additional actions upon animation schedule.
	/// </remarks>
	/// <param name="pGroup">A pointer to an animation group, which has been scheduled.</param>
	virtual void OnAfterSchedule(CAnimationGroup* pGroup);

	/// <summary>
	/// A helper that cleans up the group.
	/// </summary>
	/// <remarks>
	/// This method removes all transitions and keyframes from the specified group.
	/// </remarks>
	/// <param name="pGroup">A pointer to animation group to clean.</param>
	void CleanUpGroup(CAnimationGroup* pGroup);
};

//----------------------------
// Event handler definitions
//----------------------------
/// <summary>
/// Implements a callback, which is called by Animation API when a status of animation manager changed.
/// </summary>
/// <remarks>
/// This event handler is created and passed to IUIAnimationManager::SetManagerEventHandler method,
/// when you call CAnimationController::EnableAnimationManagerEvent.
/// </remarks>
class CAnimationManagerEventHandler : public CUIAnimationManagerEventHandlerBase<CAnimationManagerEventHandler>
{
	CAnimationController* m_pAnimationController;

public:
	/// <summary>
	/// Constructs a CAnimationManagerEventHandler object.
	/// </summary>
	CAnimationManagerEventHandler() : m_pAnimationController(NULL)
	{
	}

	/// <summary>
	/// Creates an instance of CAnimationManagerEventHandler object.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppManagerEventHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// status updates to an animation manager.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationManagerEventHandler **ppManagerEventHandler)
	{
		CAnimationManagerEventHandler *pManagerEventHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppManagerEventHandler, &pManagerEventHandler);
		if (SUCCEEDED(hr))
		{
			pManagerEventHandler->SetAnimationController (pAnimationController);
		}

		return hr;
	}

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Called when a status of animation manager has changed.
	/// </summary>
	/// <returns>
	/// Current implementation always returns S_OK;
	/// </returns>
	/// <param name="newStatus">New status.</param>
	/// <param name="previousStatus">Previous status.</param>
	IFACEMETHOD(OnManagerStatusChanged)(_In_ UI_ANIMATION_MANAGER_STATUS newStatus, _In_ UI_ANIMATION_MANAGER_STATUS previousStatus)
	{
		if (m_pAnimationController != NULL)
		{
			m_pAnimationController->OnAnimationManagerStatusChanged(newStatus, previousStatus);
		}

		return S_OK;
	}
};

/// <summary>
/// Implements a call back, which is called by Animation API when the value of an animation variable changes.
/// </summary>
/// <remarks>
/// This event handler is created and passed to IUIAnimationVariable::SetVariableChangeHandler method,
/// when you call CAnimationVariable::EnableValueChangedEvent or CAnimationBaseObject::EnableValueChangedEvent
/// (which enables this event for all animation variables encapsulated in an animation object).
/// </remarks>

class CAnimationVariableChangeHandler : public CUIAnimationVariableChangeHandlerBase<CAnimationVariableChangeHandler>
{
	CAnimationController* m_pAnimationController;

public:
	/// <summary>
	/// Constructs a CAnimationVariableChangeHandler object.
	/// </summary>
	CAnimationVariableChangeHandler() : m_pAnimationController(NULL)
	{
	}

	/// <summary>
	/// Creates an instance of CAnimationVariableChangeHandler object.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// variable change events.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationVariableChangeHandler **ppHandler)
	{
		CAnimationVariableChangeHandler *pHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppHandler, &pHandler);
		if (SUCCEEDED(hr))
		{
			pHandler->SetAnimationController (pAnimationController);
		}

		return hr;
	}

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Called when a value of an animation variable has changed.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="storyboard">The storyboard that is animating the variable.</param>
	/// <param name="variable">The animation variable that was updated.</param>
	/// <param name="newValue">The new value.</param>
	/// <param name="previousValue">The previous value.</param>
	IFACEMETHOD(OnValueChanged)
    (
        _In_ IUIAnimationStoryboard *storyboard,
        _In_ IUIAnimationVariable *variable,
        _In_ DOUBLE newValue,
        _In_ DOUBLE previousValue
    )
	{
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of OnValueChanged event handler has not been initialized.\n"));
			return E_FAIL;
		}

		if (storyboard == NULL && variable != NULL)
		{
			(void)variable->GetCurrentStoryboard(&storyboard);
		}

		CAnimationGroup* pGroup = NULL;
		CAnimationBaseObject* pObject = NULL;

		if (storyboard != NULL)
		{
			pGroup = m_pAnimationController->FindAnimationGroup(storyboard);
			if (pGroup == NULL)
			{
				TRACE(_T("Got OnValueChanged event for a storyboard, which does not belong to any Animation Group.\n"));
				return E_FAIL;
			}

			pObject = pGroup->FindAnimationObject(variable);

			if (pObject == NULL)
			{
				TRACE(_T("Got OnValueChanged event from an animation variable, which does not belong to any Animation Group.\n"));
				return E_FAIL;
			}
		}
		else
		{
			m_pAnimationController->FindAnimationObject(variable, &pObject, &pGroup);
		}

		m_pAnimationController->OnAnimationValueChanged(pGroup, pObject, variable, newValue, previousValue);
		return S_OK;
	}
};

/// <summary>
/// Implements a call back, which is called by Animation API when the value of an animation variable changes.
/// </summary>
/// <remarks>
/// This event handler is created and passed to IUIAnimationVariable::SetVariableIntegerChangeHandler method,
/// when you call CAnimationVariable::EnableIntegerValueChangedEvent or CAnimationBaseObject::EnableIntegerValueChangedEvent
/// (which enables this event for all animation variables encapsulated in an animation object).
/// </remarks>
class CAnimationVariableIntegerChangeHandler : public CUIAnimationVariableIntegerChangeHandlerBase<CAnimationVariableIntegerChangeHandler>
{
	CAnimationController* m_pAnimationController;

public:
	/// <summary>
	/// Constructs a CAnimationVariableIntegerChangeHandler object.
	/// </summary>
	CAnimationVariableIntegerChangeHandler () : m_pAnimationController(NULL)
	{
	}

	/// <summary>
	/// Creates an instance of CAnimationVariableIntegerChangeHandler callback.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// variable integer change events.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationVariableIntegerChangeHandler **ppHandler)
	{
		CAnimationVariableIntegerChangeHandler *pHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppHandler, &pHandler);
		if (SUCCEEDED(hr))
		{
			pHandler->SetAnimationController (pAnimationController);
		}

		return hr;
	}

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Called when a value of an animation variable has changed.
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
	/// <param name="storyboard">The storyboard that is animating the variable.</param>
	/// <param name="variable">The animation variable that was updated.</param>
	/// <param name="newValue">The new rounded value.</param>
	/// <param name="previousValue">The previous rounded value.</param>
	IFACEMETHOD(OnIntegerValueChanged)
    (
        _In_ IUIAnimationStoryboard *storyboard,
        _In_ IUIAnimationVariable *variable,
        _In_ INT32 newValue,
        _In_ INT32 previousValue
    )
	{
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of OnIntegerValueChanged event handler has not been initialized.\n"));
			return E_FAIL;
		}

		if (storyboard == NULL && variable != NULL)
		{
			(void)variable->GetCurrentStoryboard(&storyboard);
		}

		CAnimationGroup* pGroup = NULL;
		CAnimationBaseObject* pObject = NULL;

		if (storyboard != NULL)
		{
			pGroup = m_pAnimationController->FindAnimationGroup(storyboard);
			if (pGroup == NULL)
			{
				TRACE(_T("Got OnIntegerValueChanged event for a storyboard, which does not belong to any Animation Group.\n"));
				return E_FAIL;
			}

			pObject = pGroup->FindAnimationObject(variable);

			if (pObject == NULL)
			{
				TRACE(_T("Got OnIntegerValueChanged event from an animation variable, which does not belong to any Animation Group.\n"));
				return E_FAIL;
			}
		}
		else
		{
			m_pAnimationController->FindAnimationObject(variable, &pObject, &pGroup);
		}

		m_pAnimationController->OnAnimationIntegerValueChanged(pGroup, pObject, variable, newValue, previousValue);
		return S_OK;
	}
};

/// <summary>
/// Implements a callback, which is called by Animation API when storyboard's status is changed or storyboard is updated.
/// </summary>
/// <remarks>
/// This event handler is created and passed to IUIAnimationStoryboard::SetStoryboardEventHandler method,
/// when you call CAnimationController::EnableStoryboardEventHandler.
/// </remarks>
class CAnimationStoryboardEventHandler : public CUIAnimationStoryboardEventHandlerBase<CAnimationStoryboardEventHandler>
{
	CAnimationController* m_pAnimationController;
public:
	/// <summary>
	/// Constructs a CAnimationStoryboardEventHandler object.
	/// </summary>
	CAnimationStoryboardEventHandler() : m_pAnimationController(NULL)
	{
	}

	/// <summary>
	/// Creates an instance of CAnimationStoryboardEventHandler callback.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// storyboard events.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationStoryboardEventHandler **ppHandler)
	{
		CAnimationStoryboardEventHandler* pHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppHandler, &pHandler);
		if (SUCCEEDED(hr))
		{
			pHandler->SetAnimationController (pAnimationController);
		}

		return hr;
	}

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Handles OnStoryboardStatusChanged events, which occur when a storyboard's status changes
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
	/// <param name="storyboard">A pointer to storyboard whose status has changed.</param>
	/// <param name="newStatus">Specifies new storyboard status.</param>
	/// <param name="previousStatus">Specifies previous storyboard status.</param>
    IFACEMETHOD(OnStoryboardStatusChanged)
    (
        _In_ IUIAnimationStoryboard *storyboard,                        // The storyboard that changed status
        _In_ UI_ANIMATION_STORYBOARD_STATUS newStatus,                  // The new status
        _In_ UI_ANIMATION_STORYBOARD_STATUS previousStatus              // The previous status
    )
	{
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of OnStoryboardStatusChanged event handler has not been initialized.\n"));
			return E_FAIL;
		}

		CAnimationGroup* pGroup = m_pAnimationController->FindAnimationGroup(storyboard);
		if (pGroup == NULL)
		{
			TRACE(_T("Got an event from storyboard, which does not belong to any Animation Group.\n"));
			return E_FAIL;
		}
		m_pAnimationController->OnStoryboardStatusChanged(pGroup, newStatus, previousStatus);
		return S_OK;
	}

	/// <summary>
    /// Handles OnStoryboardUpdated events, which occur when a storyboard is updated
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
	/// <param name="storyboard">A pointer to storyboard, which was updated.</param>
    IFACEMETHOD(OnStoryboardUpdated)
    (
        _In_ IUIAnimationStoryboard *storyboard                         // The storyboard that was updated
    )
	{
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of OnStoryboardUpdated event handler has not been initialized.\n"));
			return E_FAIL;
		}

		CAnimationGroup* pGroup = m_pAnimationController->FindAnimationGroup(storyboard);
		if (pGroup == NULL)
		{
			TRACE(_T("Got an event from storyboard, which does not belong to any Animation Group.\n"));
			return E_FAIL;
		}
		m_pAnimationController->OnStoryboardUpdated(pGroup);
		return S_OK;
	}
};

class PriorityComparisonMethodCancel
{
public:
	static BOOL Method(CAnimationController* pController, CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect)
	{
		return pController->OnHasPriorityCancel(pGroupScheduled, pGroupNew, priorityEffect);
	}
};

class PriorityComparisonMethodConclude
{
public:
	static BOOL Method(CAnimationController* pController, CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect)
	{
		return pController->OnHasPriorityConclude(pGroupScheduled, pGroupNew, priorityEffect);
	}
};

class PriorityComparisonMethodTrim
{
public:
	static BOOL Method(CAnimationController* pController, CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect)
	{
		return pController->OnHasPriorityTrim(pGroupScheduled, pGroupNew, priorityEffect);
	}
};

class PriorityComparisonMethodCompress
{
public:
	static BOOL Method(CAnimationController* pController, CAnimationGroup* pGroupScheduled, CAnimationGroup* pGroupNew, UI_ANIMATION_PRIORITY_EFFECT priorityEffect)
	{
		return pController->OnHasPriorityCompress(pGroupScheduled, pGroupNew, priorityEffect);
	}
};

#pragma warning(push)
#pragma warning(disable:4634)
/// <summary>
/// Implements a call back, which is called by Animation API when it needs to resolve scheduling conflicts.
/// </summary>
/// <remarks>
/// This event handler is created and passed to one of IUIAnimationManager's Set methods that deal with priority comparisons,
/// when you call CAnimationController::EnablePriorityComparisonHandler.
/// </remarks>
template <class PriorityComparisonMethod>
class CUIAnimationPriorityComparisonHandler : public CUIAnimationPriorityComparisonBase<CUIAnimationPriorityComparisonHandler<PriorityComparisonMethod>>
{
	CAnimationController* m_pAnimationController;
public:
	/// <summary>
	/// Creates an instance of CUIAnimationPriorityComparisonHandler callback.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppManagerEventHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// priority comparison events.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationPriorityComparison **ppHandler)
	{
		CUIAnimationPriorityComparisonHandler<PriorityComparisonMethod>* pHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase<IUIAnimationPriorityComparison, CUIAnimationPriorityComparisonHandler<PriorityComparisonMethod>>::CreateInstance(ppHandler, &pHandler);
		if (SUCCEEDED(hr))
		{
			pHandler->SetAnimationController (pAnimationController);
		}

		return hr;
	}

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Determines the relative priority between a scheduled storyboard and a new storyboard.
	/// </summary>
	/// <returns>
	/// Should return S_OK if pStoryboardNew has priority; should return S_FALSE if pStoryboardScheduled has priority.
	/// </returns>
	/// <param name="pStoryboardScheduled">Currently scheduled storyboard.</param>
	/// <param name="pStoryboardNew">New storyboard that conflicts with scheduled storyboard.</param>
	/// <param name="priorityEffect">Potential effect on attempt to schedule storyboard if HasPriority returns S_FALSE.</param>
	IFACEMETHOD(HasPriority)
    (
        _In_ IUIAnimationStoryboard *pStoryboardScheduled,
        _In_ IUIAnimationStoryboard *pStoryboardNew,
        _In_ UI_ANIMATION_PRIORITY_EFFECT priorityEffect
    )
	{
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of HasPriority event handler has not been initialized.\n"));
			return E_FAIL;
		}

		CAnimationGroup* pGroupScheduled = m_pAnimationController->FindAnimationGroup(pStoryboardScheduled);
		if (pGroupScheduled == NULL)
		{
			TRACE(_T("Got a HasPriority event for a storyboard that does not belong to any group.\n"));
			return E_FAIL;
		}

		CAnimationGroup* pGroupNew = m_pAnimationController->FindAnimationGroup(pStoryboardNew);
		if (pGroupNew == NULL)
		{
			TRACE(_T("Got a HasPriority event for a storyboard that does not belong to any group.\n"));
			return E_FAIL;
		}
		BOOL bResult = PriorityComparisonMethod::Method(m_pAnimationController, pGroupScheduled, pGroupNew, priorityEffect);
		return bResult ? S_OK : S_FALSE;
	}
};
#pragma warning(pop)

/// <summary>
/// Implements a call back, which is called by Animation API when timing events occur.
/// </summary>
/// <remarks>
/// This event handler is created and passed to IUIAnimationTimer::SetTimerEventHandler when you call
/// CAnimationController::EnableAnimationTimerEventHandler.
/// </remarks>
class CAnimationTimerEventHandler : public CUIAnimationTimerEventHandlerBase<CAnimationTimerEventHandler>
{
	CAnimationController* m_pAnimationController;
public:

	/// <summary>
	/// Creates an instance of CAnimationTimerEventHandler callback.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// </returns>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	/// <param name="ppTimerEventHandler">Output. If the method succeeds it contains a pointer to COM object that will handle
	/// animation timer events.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CAnimationController* pAnimationController, IUIAnimationTimerEventHandler **ppTimerEventHandler)
    {
        CAnimationTimerEventHandler* pHandler = NULL;
        HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppTimerEventHandler, &pHandler);

        if (SUCCEEDED(hr))
        {
            pHandler->SetAnimationController (pAnimationController);
        }

        return hr;
    }

	/// <summary>
	/// Stores a pointer to animation controller to route events.
	/// </summary>
	/// <param name="pAnimationController">A pointer to animation controller, which will receive events.</param>
	void SetAnimationController(CAnimationController* pAnimationController)
	{
		m_pAnimationController = pAnimationController;
	}

	/// <summary>
	/// Handles events that occur before an animation update begins.
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
	IFACEMETHOD(OnPreUpdate)()
    {
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of CAnimationTimerEventHandler event handler has not been initialized.\n"));
			return E_FAIL;
		}

        m_pAnimationController->OnAnimationTimerPreUpdate();
        return S_OK;
    }

	/// <summary>
	/// Handles events that occur after an animation update is finished.
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
    IFACEMETHOD(OnPostUpdate)()
    {
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of CAnimationTimerEventHandler event handler has not been initialized.\n"));
			return E_FAIL;
		}

        m_pAnimationController->OnAnimationTimerPostUpdate();
        return S_OK;
    }

	/// <summary>
	/// Handles events that occur when the rendering frame rate for an animation falls below the minimum desirable frame rate.
	/// </summary>
	/// <returns>
	/// S_OK if the method succeeds; otherwise E_FAIL.
	/// </returns>
    IFACEMETHOD(OnRenderingTooSlow)(_In_ UINT32 fps)
    {
		if (m_pAnimationController == NULL)
		{
			TRACE(_T("An instance of CAnimationTimerEventHandler event handler has not been initialized.\n"));
			return E_FAIL;
		}

        m_pAnimationController->OnAnimationTimerRenderingTooSlow(fps);
        return S_OK;
    }
};

//---------------------------------
// Transition class definitions
//---------------------------------
/// <summary>
/// Implements an accelerate-decelerate transition.
/// </summary>
/// <remarks>
/// During an accelerate-decelerate transition, the animation variable speeds up and then slows down over the
/// duration of the transition, ending at a specified value. You can control how quickly the variable accelerates and
/// decelerates independently, by specifying different acceleration and deceleration ratios.
/// When the initial velocity is zero, the acceleration ratio is the fraction of the duration that the variable will
/// spend accelerating; likewise with the deceleration ratio. If the initial velocity is non-zero, it is the
/// fraction of the time between the velocity reaching zero and the end of transition. The acceleration ratio and
/// the deceleration ratio should sum to a maximum of 1.0.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CAccelerateDecelerateTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object.
	/// </summary>
	/// <param name="duration">The duration of the transition. </param>
	/// <param name="finalValue">The value of the animation variable at the end of the transition. </param>
	/// <param name="accelerationRatio">The ratio of the time spent accelerating to the duration.</param>
	/// <param name="decelerationRatio">The ratio of the time spent decelerating to the duration. </param>
	CAccelerateDecelerateTransition(UI_ANIMATION_SECONDS duration, DOUBLE finalValue, DOUBLE accelerationRatio = 0.3,
		DOUBLE decelerationRatio = 0.3) : m_duration(duration), m_finalValue(finalValue),
		m_accelerationRatio(accelerationRatio), m_decelerationRatio(decelerationRatio)
	{
		m_type = ACCELERATE_DECELERATE;
	}

public:
	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_finalValue;

	/// <summary>
	/// The ratio of the time spent accelerating to the duration.
	/// </summary>
	DOUBLE m_accelerationRatio;

	/// <summary>
	/// The ratio of the time spent decelerating to the duration.
	/// </summary>
	DOUBLE m_decelerationRatio;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateAccelerateDecelerateTransition(m_duration, m_finalValue, m_accelerationRatio,
				m_decelerationRatio, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a constant transition.
/// </summary>
/// <remarks>
/// During a constant transition, the value of an animation variable remains at the initial value over the duration
/// of the transition.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CConstantTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object and initializes its duration.
	/// </summary>
	/// <param name="duration">The duration of the transition. </param>
	CConstantTransition (UI_ANIMATION_SECONDS duration) : m_duration(duration)
	{
		m_type = CONSTANT;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateConstantTransition(m_duration, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a cubic transition.
/// </summary>
/// <remarks>
/// During a cubic transition, the value of the animation variable changes from its initial value to a specified
/// final value over the duration of the transition, ending at a specified velocity.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CCubicTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object and initializes its parameters.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="finalValue">The value of the animation variable at the end of the transition.</param>
	/// <param name="finalVelocity">The velocity of the variable at the end of the transition.</param>
	CCubicTransition(UI_ANIMATION_SECONDS duration, DOUBLE finalValue, DOUBLE finalVelocity) : m_duration(duration),
		m_dblFinalValue(finalValue), m_dblFinalVelocity(finalVelocity)
	{
		m_type = CUBIC;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// The velocity of the variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalVelocity;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateCubicTransition(m_duration, m_dblFinalValue, m_dblFinalVelocity, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a discrete transition.
/// </summary>
/// <remarks>
/// During a discrete transition, the animation variable remains at the initial value for a specified delay time,
/// then switches instantaneously to a specified final value and remains at that value for a given hold time.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CDiscreteTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a discrete transition object and initializes its parameters.
	/// </summary>
	/// <param name="delay">The amount of time by which to delay the instantaneous switch to the final value.</param>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition. </param>
	/// <param name="hold">The amount of time by which to hold the variable at its final value.</param>
	CDiscreteTransition(UI_ANIMATION_SECONDS delay, DOUBLE dblFinalValue, UI_ANIMATION_SECONDS hold) : m_delay(delay),
		m_dblFinalValue(dblFinalValue), m_hold(hold)
	{
		m_type = DISCRETE;
	}

	/// <summary>
	/// The amount of time by which to delay the instantaneous switch to the final value.
	/// </summary>
	UI_ANIMATION_SECONDS m_delay;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// The amount of time by which to hold the variable at its final value.
	/// </summary>
	UI_ANIMATION_SECONDS m_hold;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateDiscreteTransition(m_delay, m_dblFinalValue, m_hold, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates an instantaneous transition.
/// </summary>
/// <remarks>
/// During an instantaneous transition, the value of the animation variable changes instantly from its current
/// value to a specified final value. The duration of this transition is always zero.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CInstantaneousTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object and initializes its final value.
	/// </summary>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition.</param>
	CInstantaneousTransition(DOUBLE dblFinalValue) : m_dblFinalValue(dblFinalValue)
	{
		m_type = INSTANTENEOUS;
	}

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateInstantaneousTransition(m_dblFinalValue, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a linear transition.
/// </summary>
/// <remarks>
/// During a linear transition, the value of the animation variable transitions linearly from its initial value to a specified final value.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CLinearTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a linear transition object and initializes it with duration and final value.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition.</param>
	CLinearTransition(UI_ANIMATION_SECONDS duration, DOUBLE dblFinalValue) : m_duration(duration), m_dblFinalValue(dblFinalValue)
	{
		m_type = LINEAR;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateLinearTransition(m_duration, m_dblFinalValue, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a linear-speed transition.
/// </summary>
/// <remarks>
/// During a linear-speed transition, the value of the animation variable changes at a specified rate.
/// The duration of the transition is determined by the difference between the initial value and the specified final value.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CLinearTransitionFromSpeed : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a linear-speed transition object and initializes it with speed and final value.
	/// </summary>
	/// <param name="dblSpeed">The absolute value of the variable's velocity.</param>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition.</param>
	CLinearTransitionFromSpeed(DOUBLE dblSpeed, DOUBLE dblFinalValue) : m_dblSpeed(dblSpeed), m_dblFinalValue(dblFinalValue)
	{
		m_type = LINEAR_FROM_SPEED;
	}

	/// <summary>
	/// The absolute value of the variable's velocity.
	/// </summary>
	DOUBLE m_dblSpeed;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateLinearTransitionFromSpeed(m_dblSpeed, m_dblFinalValue, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a smooth-stop transition.
/// </summary>
/// <remarks>
/// A smooth-stop transition slows down as it approaches a given final value, and reaches it with a velocity of zero.
/// The duration of the transition is determined by the initial velocity, the difference between the initial and final values,
/// and the specified maximum duration. If there is no solution consisting of a single parabolic arc,
/// this method creates a cubic transition.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CSmoothStopTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a smooth-stop transition and initializes its maximum duration and final value.
	/// </summary>
	/// <param name="maximumDuration">The maximum duration of the transition.</param>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition.</param>
	CSmoothStopTransition(UI_ANIMATION_SECONDS maximumDuration, DOUBLE dblFinalValue) : m_maximumDuration(maximumDuration),
		m_dblFinalValue(dblFinalValue)
	{
		m_type = SMOOTH_STOP;
	}

	/// <summary>
	/// The maximum duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_maximumDuration;

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateSmoothStopTransition(m_maximumDuration, m_dblFinalValue, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a parabolic-acceleration transition.
/// </summary>
/// <remarks>
/// During a parabolic-acceleration transition, the value of the animation variable changes from the initial value
/// to the final value ending at a specified velocity. You can control how quickly the variable reaches the final
/// value by specifying the rate of acceleration.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CParabolicTransitionFromAcceleration : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a parabolic-acceleration transition and initializes it with specified parameters.
	/// </summary>
	/// <param name="dblFinalValue">The value of the animation variable at the end of the transition. </param>
	/// <param name="dblFinalVelocity">The velocity of the animation variable at the end of the transition. </param>
	/// <param name="dblAcceleration">The acceleration of the animation variable during the transition.</param>
	CParabolicTransitionFromAcceleration(DOUBLE dblFinalValue, DOUBLE dblFinalVelocity, DOUBLE dblAcceleration) :
	  m_dblFinalValue(dblFinalValue), m_dblFinalVelocity(dblFinalVelocity), m_dblAcceleration(dblAcceleration)
	{
		m_type = PARABOLIC_FROM_ACCELERATION;
	}

	/// <summary>
	/// The value of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalValue;

	/// <summary>
	/// The velocity of the animation variable at the end of the transition.
	/// </summary>
	DOUBLE m_dblFinalVelocity;

	/// <summary>
	/// The acceleration of the animation variable during the transition.
	/// </summary>
	DOUBLE m_dblAcceleration;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateParabolicTransitionFromAcceleration(m_dblFinalValue, m_dblFinalVelocity, m_dblAcceleration, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a reversal transition.
/// </summary>
/// <remarks>
/// A reversal transition smoothly changes direction over a given duration.
/// The final value will be the same as the initial value and the final velocity will be the negative of the initial velocity.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CReversalTransition : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a reversal transition object and initializes its duration.
	/// </summary>
	/// <param name="duration">The duration of the transition. </param>
	CReversalTransition(UI_ANIMATION_SECONDS duration) : m_duration(duration)
	{
		m_type = REVERSAL;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateReversalTransition(m_duration, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a sinusoidal-range transition, with a given range of oscillation.
/// </summary>
/// <remarks>
/// The value of the animation variable fluctuates between the specified minimum and maximum values over the entire
/// duration of a sinusoidal-range transition. The slope parameter is used to disambiguate between the two possible
/// sine waves specified by the other parameters.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CSinusoidalTransitionFromRange : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="dblMinimumValue">The value of the animation variable at a trough of the sinusoidal wave.</param>
	/// <param name="dblMaximumValue">The value of the animation variable at a peak of the sinusoidal wave.</param>
	/// <param name="period">The period of oscillation of the sinusoidal wave in seconds. </param>
	/// <param name="slope">The slope at the start of the transition. </param>
	CSinusoidalTransitionFromRange(UI_ANIMATION_SECONDS duration, DOUBLE dblMinimumValue, DOUBLE dblMaximumValue,
		UI_ANIMATION_SECONDS period, UI_ANIMATION_SLOPE slope) : m_duration(duration), m_dblMinimumValue(dblMinimumValue),
		m_dblMaximumValue(dblMaximumValue), m_period(period), m_slope(slope)
	{
		m_type = SINUSOIDAL_FROM_RANGE;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;
	/// <summary>
	/// The value of the animation variable at a trough of the sinusoidal wave.
	/// </summary>
	DOUBLE m_dblMinimumValue;
	/// <summary>
	/// The value of the animation variable at a peak of the sinusoidal wave.
	/// </summary>
	DOUBLE m_dblMaximumValue;
	/// <summary>
	/// The period of oscillation of the sinusoidal wave in seconds.
	/// </summary>
	UI_ANIMATION_SECONDS m_period;
	/// <summary>
	/// The slope at the start of the transition.
	/// </summary>
	UI_ANIMATION_SLOPE m_slope;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateSinusoidalTransitionFromRange(m_duration, m_dblMinimumValue, m_dblMaximumValue,
				m_period, m_slope, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

/// <summary>
/// Encapsulates a sinusoidal-velocity transition, with an amplitude determined by the animation variable's initial velocity..
/// </summary>
/// <remarks>
/// The value of the animation variable oscillates around the initial value over the entire duration of a
/// sinusoidal-range transition. The amplitude of the oscillation is determined by the animation variable's velocity
/// when the transition begins.
/// Because all transitions are cleared automatically, it's recommended to allocated them using operator new.
/// The encapsulated IUIAnimationTransition COM object is created by CAnimationController::AnimateGroup, until then
/// it's NULL. Changing member variables after creation of this COM object has no effect.
/// </remarks>
class CSinusoidalTransitionFromVelocity : public CBaseTransition
{
public:
	/// <summary>
	/// Constructs a transition object.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="period">The period of oscillation of the sinusoidal wave in seconds. </param>
	CSinusoidalTransitionFromVelocity(UI_ANIMATION_SECONDS duration, UI_ANIMATION_SECONDS period) : m_duration(duration), m_period(period)
	{
		m_type = SINUSOIDAL_FROM_VELOCITY;
	}

	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS m_duration;
	/// <summary>
	/// The period of oscillation of the sinusoidal wave in seconds.
	/// </summary>
	UI_ANIMATION_SECONDS m_period;

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <returns>
	/// TRUE if transition is created successfully; otherwise FALSE.
	/// </returns>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		UNREFERENCED_PARAMETER(pFactory);

		Clear();

		if (pLibrary != NULL)
		{
			HRESULT hr = pLibrary->CreateSinusoidalTransitionFromVelocity(m_duration, m_period, &m_transition);
			return SUCCEEDED(hr);
		}
		return FALSE;
	}
};

//---------------------------------
// Support for custom transitions
//---------------------------------
/// <summary>
/// Implements a basic interpolator.
/// </summary>
/// <remarks>
/// Derive a class from CCustomInterpolator and override all necessary methods in order to implement a custom
/// interpolation algorithm. A pointer to this class should be passed as a parameter to CCustomTransition.
/// </remarks>
class CCustomInterpolator
{
protected:
	/// <summary>
	/// The duration of the transition.
	/// </summary>
	UI_ANIMATION_SECONDS	m_duration;

	/// <summary>
	/// The final value of a variable at the end of the transition.
	/// </summary>
	DOUBLE					m_finalValue;

	/// <summary>
	/// The value of the variable at the start of the transition.
	/// </summary>
	DOUBLE					m_initialValue;
	/// <summary>
	/// The velocity of the variable at the start of the transition.
	/// </summary>
	DOUBLE					m_initialVelocity;

	/// <summary>
	/// The interpolated value.
	/// </summary>
	DOUBLE					m_currentValue;
	/// <summary>
	/// The interpolated velocity.
	/// </summary>
	DOUBLE					m_currentVelocity;
public:
	/// <summary>
	/// Constructs a custom interpolator object and sets all values to default 0.
	/// </summary>
	/// <remarks>
	/// Use CCustomInterpolator::Init to initialize duration and final value later in the code.
	/// </remarks>
	CCustomInterpolator() : m_duration(0.), m_finalValue(0.), m_initialValue(0.), m_initialVelocity(0.), m_currentValue(0.),
		m_currentVelocity(0.)
	{
	}

	/// <summary>
	/// Constructs a custom interpolator object and initializes duration and velocity to specified values.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="finalValue"></param>
	CCustomInterpolator(UI_ANIMATION_SECONDS duration, DOUBLE finalValue) :
	  m_duration(duration), m_finalValue(finalValue), m_initialValue(0.), m_initialVelocity(0.), m_currentValue(0.), m_currentVelocity(0.)
	{
	}

	/// <summary>
	/// Initializes duration and final value.
	/// </summary>
	/// <param name="duration">The duration of the transition.</param>
	/// <param name="finalValue">The final value of a variable at the end of the transition.</param>
	void Init(UI_ANIMATION_SECONDS duration, DOUBLE finalValue)
	{
		m_duration = duration;
		m_finalValue = finalValue;
	}

	/// <summary>
	/// Sets the interpolator's initial value and velocity.
	/// </summary>
	/// <returns>
	/// The basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="initialValue">The value of the variable at the start of the transition.</param>
	/// <param name="initialVelocity">The velocity of the variable at the start of the transition.</param>
	virtual BOOL SetInitialValueAndVelocity(DOUBLE initialValue, DOUBLE initialVelocity)
	{
		m_initialValue = initialValue;
		m_initialVelocity = initialVelocity;
		m_currentValue = initialValue;
		m_currentVelocity = initialVelocity;
		return TRUE;
	}

	/// <summary>
    /// Sets the interpolator's duration.
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="duration">The duration of the transition.</param>
	virtual BOOL SetDuration(UI_ANIMATION_SECONDS duration)
	{
		m_duration = duration;
		return TRUE;
	}

	/// <summary>
    /// Gets the interpolator's duration.
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="duration">Output. The duration of the transition, in seconds.</param>
	virtual BOOL GetDuration(UI_ANIMATION_SECONDS *duration)
	{
		*duration = m_duration;
		return TRUE;
	}

	/// <summary>
    /// Gets the final value to which the interpolator leads.
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="value">Output. The final value of a variable at the end of the transition.</param>
	virtual BOOL GetFinalValue(DOUBLE *value)
	{
		*value = m_finalValue;
		return TRUE;
	}

	/// <summary>
    /// Interpolates the value at a given offset.
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="offset">The offset from the start of the transition.
	/// The offset is always greater than or equal to zero and less than the duration of the transition.
	/// This method is not called if the duration of the transition is zero.</param>
	/// <param name="value">Output. The interpolated value.</param>
	virtual BOOL InterpolateValue(UI_ANIMATION_SECONDS offset, DOUBLE *value)
	{
		UNREFERENCED_PARAMETER(offset);

		*value = m_currentValue;
		return TRUE;
	}

	/// <summary>
    /// Interpolates the velocity at a given offset
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="offset">The offset from the start of the transition.
	/// The offset is always greater than or equal to zero and less than or equal to the duration of the transition.
	/// This method is not called if the duration of the transition is zero. </param>
	/// <param name="velocity">Output. The velocity of the variable at the offset.</param>
	virtual BOOL InterpolateVelocity(UI_ANIMATION_SECONDS offset, DOUBLE *velocity)
	{
		UNREFERENCED_PARAMETER(offset);

		*velocity = m_currentVelocity;
		return TRUE;
	}

	/// <summary>
    /// Gets the interpolator's dependencies.
	/// </summary>
	/// <returns>
	/// Basic implementation always returns TRUE. Return FALSE from overridden implementation if you wish to fail the event.
	/// </returns>
	/// <param name="initialValueDependencies">Output. Aspects of the interpolator that depend on the initial value passed to SetInitialValueAndVelocity.</param>
	/// <param name="initialVelocityDependencies">Output. Aspects of the interpolator that depend on the initial velocity passed to SetInitialValueAndVelocity.</param>
	/// <param name="durationDependencies">Output. Aspects of the interpolator that depend on the duration passed to SetDuration.</param>
	virtual BOOL GetDependencies(UI_ANIMATION_DEPENDENCIES *initialValueDependencies,
		UI_ANIMATION_DEPENDENCIES *initialVelocityDependencies, UI_ANIMATION_DEPENDENCIES *durationDependencies)
	{
		*initialValueDependencies = UI_ANIMATION_DEPENDENCY_NONE;
		*initialVelocityDependencies = UI_ANIMATION_DEPENDENCY_NONE;
		*durationDependencies = UI_ANIMATION_DEPENDENCY_NONE;
		return TRUE;
	}
};

/// <summary>
/// Implements a callback, which is called by Animation API when it needs to calculate a new value of animation variable.
/// </summary>
/// <remarks>
/// This handler is created and passed to IUIAnimationTransitionFactory::CreateTransition when a CCustomTransition
/// object is being created as a part of animation initialization process (started by CAnimationController::AnimateGroup).
/// Usually you don't need to use this class directly, it just routs all events to a CCustomInterpolator-derived class, whose
/// pointer is passed to constructor of CCustomTransition.
/// </remarks>
class CInterpolatorBase : public CUIAnimationInterpolatorBase<CInterpolatorBase>
{
	CCustomInterpolator* m_pInterpolator;
public:
	/// <summary>
	/// Constructs the CInterpolatorBase object.
	/// </summary>
	CInterpolatorBase() : m_pInterpolator(NULL)
	{
	}

	/// <summary>
	/// Stores  a pointer to custom interpolator, which will be handling events.
	/// </summary>
	/// <param name="pInterpolator">A pointer to custom interpolator.</param>
	void SetCustomInterpolator(CCustomInterpolator* pInterpolator)
	{
		m_pInterpolator = pInterpolator;
	}

	/// <summary>
	/// Creates an instance of CInterpolatorBase and stores a pointer to custom interpolator, which will be handling events.
	/// </summary>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.
	/// <param name="pInterpolator">A pointer to custom interpolator.</param>
	/// <param name="ppHandler">Output. Contains a pointer to instance of CInterpolatorBase when the function returns.</param>
	static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(CCustomInterpolator* pInterpolator, IUIAnimationInterpolator **ppHandler)
	{
		CInterpolatorBase* pHandler = NULL;
		HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppHandler, &pHandler);

		if (SUCCEEDED(hr) && pHandler != NULL)
		{
			pHandler->SetCustomInterpolator(pInterpolator);
		}

		return hr;
	}

	/// <summary>
	/// Sets the interpolator's initial value and velocity.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the SetInitialValueAndVelocity method.
	/// </returns>
	/// <param name="initialValue">The value of the variable at the start of the transition.</param>
	/// <param name="initialVelocity">The velocity of the variable at the start of the transition.</param>
    IFACEMETHOD(SetInitialValueAndVelocity)(_In_ DOUBLE initialValue, _In_ DOUBLE initialVelocity)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->SetInitialValueAndVelocity(initialValue, initialVelocity))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Sets the interpolator's duration
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the SetDuration method.
	/// </returns>
	/// <param name="duration">The duration of the transition.</param>
    IFACEMETHOD(SetDuration)(_In_ UI_ANIMATION_SECONDS duration)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->SetDuration(duration))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Gets the interpolator's duration.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the GetDuration method.
	/// </returns>
	/// <param name="duration">Output. The duration of the transition, in seconds.</param>
    IFACEMETHOD(GetDuration)(_Out_ UI_ANIMATION_SECONDS *duration)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->GetDuration(duration))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Gets the final value to which the interpolator leads.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the GetFinalValue method.
	/// </returns>
	/// <param name="value">Output. The final value of a variable at the end of the transition.</param>
    IFACEMETHOD(GetFinalValue)(_Out_ DOUBLE *value)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->GetFinalValue(value))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Interpolates the value at a given offset
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the InterpolateValue method.
	/// </returns>
	/// <param name="offset">The offset from the start of the transition.
	/// The offset is always greater than or equal to zero and less than the duration of the transition.
	/// This method is not called if the duration of the transition is zero.</param>
	/// <param name="value">Output. The interpolated value.</param>
    IFACEMETHOD(InterpolateValue)(_In_ UI_ANIMATION_SECONDS offset, _Out_ DOUBLE *value)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->InterpolateValue(offset, value))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Interpolates the velocity at a given offset
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the InterpolateVelocity method.
	/// </returns>
	/// <param name="offset">The offset from the start of the transition.
	/// The offset is always greater than or equal to zero and less than or equal to the duration of the transition.
	/// This method is not called if the duration of the transition is zero. </param>
	/// <param name="velocity">Output. The velocity of the variable at the offset.</param>
    IFACEMETHOD(InterpolateVelocity)(_In_ UI_ANIMATION_SECONDS offset, _Out_ DOUBLE *velocity)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->InterpolateVelocity(offset, velocity))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	/// <summary>
    /// Gets the interpolator's dependencies.
	/// </summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. It returns E_FAIL if CCustomInterpolator is not
	/// set, or custom implementation returns FALSE from the GetDependencies method.
	/// </returns>
	/// <param name="initialValueDependencies">Output. Aspects of the interpolator that depend on the initial value passed to SetInitialValueAndVelocity.</param>
	/// <param name="initialVelocityDependencies">Output. Aspects of the interpolator that depend on the initial velocity passed to SetInitialValueAndVelocity.</param>
	/// <param name="durationDependencies">Output. Aspects of the interpolator that depend on the duration passed to SetDuration.</param>
    IFACEMETHOD(GetDependencies)(_Out_ UI_ANIMATION_DEPENDENCIES *initialValueDependencies, _Out_ UI_ANIMATION_DEPENDENCIES *initialVelocityDependencies,
        _Out_ UI_ANIMATION_DEPENDENCIES *durationDependencies)
	{
		if (m_pInterpolator == NULL || !m_pInterpolator->GetDependencies(initialValueDependencies, initialVelocityDependencies, durationDependencies))
		{
			return E_FAIL;
		}
		return S_OK;
	}
};

/// <summary>
/// Implements custom transition.
/// </summary>
/// <remarks>
/// The CCustomTransitions class allows developers to implement custom transitions. It's created and used
/// as a standard transition, but its constructor accepts as parameter a pointer to a custom interpolator.
/// Perform the following steps to use custom transitions:
/// 1. Derive a class from CCustomInterpolator and implement at least InterpolateValue method.
/// 2. Ensure that the lifetime of custom interpolator object must be longer than duration of animation where it's used.
/// 3. Instantiate (using operator new) a CCustomTransition object and pass a pointer to custom interpolator in the constructor.
/// 4. Call CCustomTransition::SetInitialValue and CCustomTransition::SetInitialVelocity if these parameters are required for custom interpolation.
/// 5. Pass the pointer to custom transition to AddTransition method of animation object, whose value should be animated with the custom algorithm.
/// 6. When the value of animation object should change Windows Animation API will call InterpolateValue (and other relevant methods) in CCustomInterpolator.
/// </remarks>
class CCustomTransition : public CBaseTransition
{
protected:
	/// <summary>
	/// Stores a pointer to a custom interpolator.
	/// </summary>
	CCustomInterpolator* 	m_pInterpolator;

	/// <summary>
	/// Specifies whether the initial value was specified with SetInitialValue.
	/// </summary>
	BOOL					m_bInitialValueSpecified;
	/// <summary>
	/// Specifies whether the initial velocity was specified with SetInitialVelocity.
	/// </summary>
	BOOL 					m_bInitialVelocitySpecified;

	/// <summary>
	/// Stores the initial value.
	/// </summary>
	DOUBLE				 	m_initialValue;
	/// <summary>
	/// Stores the initial velocity.
	/// </summary>
	DOUBLE					m_initialVelocity;
public:
	/// <summary>
	/// Constructs a custom transition object.
	/// </summary>
	/// <param name="pInterpolator">A pointer to custom interpolator.</param>
	CCustomTransition(CCustomInterpolator* pInterpolator)
	{
		m_pInterpolator = pInterpolator;
		m_bInitialValueSpecified = FALSE;
		m_bInitialVelocitySpecified = FALSE;
		m_initialValue = 0.;
		m_initialVelocity = 0.;
		m_type = CUSTOM;
	}

	/// <summary>
	/// Sets an initial value, which will be applied to an animation variable associated with this transition.
	/// </summary>
	void SetInitialValue(DOUBLE initialValue)
	{
		m_initialValue = initialValue;
		m_bInitialValueSpecified = TRUE;
	}

	/// <summary>
	/// Sets an initial velocity, which will be applied to an animation variable associated with this transition.
	/// </summary>
	void SetInitialVelocity(DOUBLE initialVelocity)
	{
		m_initialVelocity = initialVelocity;
		m_bInitialVelocitySpecified = TRUE;
	}

	/// <summary>
	/// Calls the transition library to create encapsulated transition COM object.
	/// </summary>
	/// <remarks>
	/// This method also can set initial value and initial velocity to be applied to an animation variable, which is
	/// associated with this transition. For this purpose you have to call SetInitialValue and SetInitialVelocity before
	/// the framework creates the encapsulated transition COM object (it happens when you call CAnimationController::AnimateGroup).
	/// </remarks>
	/// <param name="pLibrary">A pointer to transition library, which is responsible for creation of standard transitions.</param>
	/// <param name="pFactory">A pointer to transition factory, which is responsible for creation of custom transitions.</param>
	virtual BOOL Create(IUIAnimationTransitionLibrary* pLibrary, IUIAnimationTransitionFactory* pFactory)
	{
		(void)pLibrary;
		Clear();

		if (pFactory != NULL && m_pInterpolator != NULL)
		{
			ATL::CComPtr<IUIAnimationInterpolator> pHandler;

			if (FAILED(CInterpolatorBase::CreateInstance(m_pInterpolator, &pHandler)))
			{
				TRACE(_T("Failed to create custom transition.\n"));
				return FALSE;
			}

			HRESULT hr = pFactory->CreateTransition(pHandler, &m_transition);
			if (FAILED(hr) || m_transition == NULL)
			{
				TRACE1("Transition factory failed to create custom transition. Error code: %x.\n", hr);
				return FALSE;
			}

			if (m_bInitialValueSpecified)
			{
				hr = m_transition->SetInitialValue(m_initialValue);
				ASSERT(SUCCEEDED(hr));
			}

			if (m_bInitialVelocitySpecified)
			{
				hr = m_transition->SetInitialVelocity(m_initialVelocity);
				ASSERT(SUCCEEDED(hr));
			}

			return TRUE;
		}

		return FALSE;
	}
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
