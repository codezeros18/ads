#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define USER_FILE "users.txt"
#define POST_FILE "posts.txt"

typedef struct User {
    int id;
    char username[MAX_STR], password[MAX_STR], email[MAX_STR];
    struct User *next;
} User;

typedef struct Post {
    int id, userId, likes;
    char title[MAX_STR], content[MAX_STR], type[MAX_STR];
    struct Post *next;
} Post;

User *loadUsers(), *registerUser(User *head), *loginUser(User *head, int *userId);
Post *loadPosts(), *createPost(Post *head, int userId);
void saveUsers(User *head), savePosts(Post *head), viewPosts(Post *head, int userId);
void freeUsers(User *head), freePosts(Post *head);
Post *deletePost(Post *head, int postId);  // Function to delete a post
void likePost(Post *head, int postId);     // Function to like a post

int main() {
    User *users = loadUsers();
    Post *posts = loadPosts();
    int userId = -1, choice, postId;

    while (1) {
        if (userId == -1) {
            printf("\n=====================================\n");
            printf("              Home Menu              \n");
            printf("=====================================\n");
            printf(" 1. Register\n");
            printf(" 2. Login\n");
            printf(" 3. Exit\n");
            printf("=====================================\n");
            printf("> ");
            scanf("%d", &choice); getchar();
            if (choice == 1) { users = registerUser(users); saveUsers(users); }
            else if (choice == 2) { users = loginUser(users, &userId); }
            else { saveUsers(users); savePosts(posts); freeUsers(users); freePosts(posts); return 0; }
        } else {
            printf("\n1. Create Post\n2. View Posts\n3. Like Post\n4. Delete Post\n5. Logout\n> ");
            scanf("%d", &choice); getchar();
            if (choice == 1) { posts = createPost(posts, userId); savePosts(posts); }
            else if (choice == 2) { 
                viewPosts(posts, userId);
            }
            else if (choice == 3) {
                printf("Enter Post ID to like: ");
                scanf("%d", &postId); getchar();
                likePost(posts, postId);
                savePosts(posts);
            }
            else if (choice == 4) {
                printf("Enter Post ID to delete: ");
                scanf("%d", &postId); getchar();
                posts = deletePost(posts, postId);
                savePosts(posts);
            }
            else { userId = -1; printf("Logged out!\n"); }
        }
    }
}

Post *deletePost(Post *head, int postId) {
    Post *prev = NULL, *cur = head;

    while (cur) {
        if (cur->id == postId) {
            if (prev) prev->next = cur->next; // Bypass the node
            else head = cur->next; // If the post to delete is the head
            free(cur);
            printf("\n✅ Post ID %d deleted successfully!\n", postId);
            return head;
        }
        prev = cur;
        cur = cur->next;
    }

    printf("\n❌ Post ID %d not found!\n", postId);
    return head;
}

void likePost(Post *head, int postId) {
    Post *cur = head;

    while (cur) {
        if (cur->id == postId) {
            cur->likes++;
            printf("\n✅ Post ID %d liked! Total Likes: %d\n", postId, cur->likes);
            return;
        }
        cur = cur->next;
    }

    printf("\n❌ Post ID %d not found!\n", postId);
}

User *registerUser(User *head) {
    User *newUser = malloc(sizeof(User));
    if (!newUser) return head;

    printf("\n=====================================\n");
    printf("           User Registration         \n");
    printf("=====================================\n");
    printf(" Enter Username: "); fgets(newUser->username, MAX_STR, stdin);
    printf(" Enter Password: "); fgets(newUser->password, MAX_STR, stdin);
    printf(" Enter Email: "); fgets(newUser->email, MAX_STR, stdin);

    newUser->username[strcspn(newUser->username, "\n")] = '\0';
    newUser->password[strcspn(newUser->password, "\n")] = '\0';
    newUser->email[strcspn(newUser->email, "\n")] = '\0';

    newUser->id = (head == NULL) ? 1 : head->id + 1;
    newUser->next = head;

    printf("\n✅ User registered successfully! ID: %d\n", newUser->id);
    return newUser;
}

User *loginUser(User *head, int *userId) {
    char username[MAX_STR], password[MAX_STR];

    printf("\n=====================================\n");
    printf("              Login                  \n");
    printf("=====================================\n");
    printf(" Enter Username: "); fgets(username, MAX_STR, stdin);
    printf(" Enter Password: "); fgets(password, MAX_STR, stdin);

    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';

    for (User *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->username, username) == 0 && strcmp(cur->password, password) == 0) {
            *userId = cur->id;
            printf("\n✅ Login Successful! Welcome, %s (ID: %d)\n", username, *userId);
            return head;
        }
    }
    printf("\n❌ Invalid login. Please try again.\n");
    return head;
}

Post *createPost(Post *head, int userId) {
    Post *newPost = malloc(sizeof(Post));
    if (!newPost) return head;

    newPost->userId = userId;
    newPost->likes = 0;

    printf("\n=====================================\n");
    printf("             Create Post             \n");
    printf("=====================================\n");
    printf(" Choose Post Type:\n");
    printf(" 1. Picture 📷\n");
    printf(" 2. Video 🎥\n");
    printf("=====================================\n");
    printf("> ");
    
    int choice;
    scanf("%d", &choice); getchar();
    strcpy(newPost->type, (choice == 1) ? "Picture" : "Video");

    printf("\n Enter Post Title: "); fgets(newPost->title, MAX_STR, stdin);
    printf(" Enter Post Content: "); fgets(newPost->content, MAX_STR, stdin);

    newPost->title[strcspn(newPost->title, "\n")] = '\0';
    newPost->content[strcspn(newPost->content, "\n")] = '\0';

    strcat(newPost->content, (choice == 1) ? ".png" : ".mp4");
    newPost->id = (head == NULL) ? 1 : head->id + 1;
    newPost->next = head;

    printf("\n✅ Post created successfully! Post ID: %d\n", newPost->id);
    return newPost;
}


void viewPosts(Post *head, int userId) {
    printf("\n=====================================\n");
    printf("              Your Posts             \n");
    printf("=====================================\n");

    int found = 0;
    for (Post *cur = head; cur; cur = cur->next) {
        if (cur->userId == userId) {
            printf(" 🆔 ID: %d\n", cur->id);
            printf(" 📌 Title: %s\n", cur->title);
            printf(" 👍 Likes: %d\n", cur->likes);
            printf(" 🎞️ Type: %s\n", cur->type);
            printf(" ✏️ Content: %s\n", cur->content);
            printf("-------------------------------------\n");
            found = 1;
        }
    }

    if (!found) {
        printf("❌ No posts found!\n");
    }
}


void saveUsers(User *head) {
    FILE *file = fopen(USER_FILE, "w");
    for (User *cur = head; cur; cur = cur->next)
        fprintf(file, "%d | %s | %s | %s\n", cur->id, cur->username, cur->password, cur->email);
    fclose(file);
}

User *loadUsers() {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return NULL;
    User *head = NULL;
    while (!feof(file)) {
        User *newUser = malloc(sizeof(User));
        if (fscanf(file, "%d | %[^|] | %[^|] | %[^\n]\n", &newUser->id, newUser->username, newUser->password, newUser->email) != 4) {
            free(newUser); break;
        }
        newUser->next = head;
        head = newUser;
    }
    fclose(file);
    return head;
}

void savePosts(Post *head) {
    FILE *file = fopen(POST_FILE, "w");
    for (Post *cur = head; cur; cur = cur->next)
        fprintf(file, "%d | %d | %s | %s | %s | %d\n", cur->id, cur->userId, cur->title, cur->content, cur->type, cur->likes);
    fclose(file);
}

Post *loadPosts() {
    FILE *file = fopen(POST_FILE, "r");
    if (!file) return NULL;
    Post *head = NULL;
    while (!feof(file)) {
        Post *newPost = malloc(sizeof(Post));
        if (fscanf(file, "%d | %d | %[^|] | %[^|] | %[^|] | %d\n", &newPost->id, &newPost->userId, newPost->title, newPost->content, newPost->type, &newPost->likes) != 6) {
            free(newPost); break;
        }
        newPost->next = head;
        head = newPost;
    }
    fclose(file);
    return head;
}

void freeUsers(User *head) { while (head) { User *tmp = head; head = head->next; free(tmp); } }
void freePosts(Post *head) { while (head) { Post *tmp = head; head = head->next; free(tmp); } }
